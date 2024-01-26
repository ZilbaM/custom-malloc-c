#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct memory_block_header {
    size_t size;
    struct memory_block_header *next;
} memory_block_header;


memory_block_header *free_list = NULL;

void print_free_list() {
    printf("\n----------------------\n");
    printf("Printing free memory blocks list\n");
    printf("----------------------\n");
    memory_block_header *block = free_list;
    int count = 1;
    if (block==NULL) {
        printf("List empty\n");
    }
    while (block != NULL) {
        printf("%i - %p - size : %i\n", count, block, (int)block->size);
        count++;
        block = block->next;
    }
}

memory_block_header* remove_element_from_free_list(memory_block_header *target) {
    memory_block_header *block = free_list;
    memory_block_header *prev_block = NULL;

    // While we don't reach the end of the free list or if the list is not empty
    while (block != NULL) {
        // We check to find the block to remove from the list
        if (block == target) {
            if (prev_block != NULL) {
                // We remove the found element from the free list, by making the pointer of the previous block jump over this one
                prev_block->next = block->next;
            } else {
                // If the block found was the first of the list, then we can reassign the first element of the list.
                free_list = block->next;
            }
            return block;
        }
        prev_block = block;
        block = block->next;
    }

    return NULL;
}

memory_block_header* biggest_address_of_free_list() {
    memory_block_header *current_block = free_list;
    memory_block_header *biggest =  current_block;
    while ((current_block = current_block->next) != NULL) {
        if (current_block > biggest) {
            biggest = current_block;
        }
    }
    return biggest;
}

void sort_free_list() {
    if (free_list == NULL) return;
    memory_block_header *new_list = NULL;
    while (free_list->next!=NULL) {
        memory_block_header *last = remove_element_from_free_list(biggest_address_of_free_list());
        last->next = new_list;
        new_list = last;
    }
    free_list->next = new_list;
}

void trim_heap() { // This function doesn't work due to an error from the brk "Cannot allocate memory", see ./brk_error for example
    sort_free_list();
    void *end_of_heap = sbrk(0);
    memory_block_header *block = free_list;
    while (block != NULL && block->next != NULL) block = block->next;
    char *block_end = (char *)(block + 1) + block->size;
    if (block_end == end_of_heap){
        if (brk(block) != (void*) -1) {
            remove_element_from_free_list(block);
        } else {
            perror("trim failed due to the brl : ");
        }
    }
}

void fuse_free_list() {
    sort_free_list();
    if (free_list == NULL) return;
    int fusion_occured;
    do{
        fusion_occured = 0;
        memory_block_header *block = free_list;
        while(block->next != NULL) {
            char *block_end = (char *)(block + 1) + block->size;
            if ((char *) block->next == block_end) {
                memory_block_header *removed = remove_element_from_free_list(block->next);
                block->size += removed->size + sizeof(memory_block_header);
                fusion_occured = 1;
                continue;
            }
            block = block->next;
        }
    } while (fusion_occured);
}

void *custom_malloc(size_t size) {
    if (size==0) {
        return NULL;
    }
    memory_block_header *block = free_list;
    memory_block_header *prev_block = NULL;

    // While we don't reach the end of the free list or if the list is not empty
    while (block != NULL) {
        // We check for the size of each free block to find a big enough block;
        if (block->size >= size) {
            if (prev_block != NULL) {
                // We remove the found element from the free list, by making the pointer of the previous block jump over this one
                prev_block->next = block->next;
            } else {
                // If the block found was the first of the list, then we can reassign the first element of the list.
                free_list = block->next;
            }
            // we return a pointer to the address of the block + 1 to skip pointing to the memory block header, and instead to the place where the data will be stored. The pointer points to a memory_block_header, so the +1 will move the pointer by the size of 1 memory_block_header and not 1 byte because of "pointer arithmetics"
            return (void *)(block + 1);
        }
        // If the free block isn't big enough, we iterate over the list to the next block
        prev_block = block;
        block = block->next;
    }


    // If no blocks in the free list are big enough for our data, we move the break with sbrk
    memory_block_header *new_block = (memory_block_header*) sbrk(size + sizeof(memory_block_header));
    
    // We verify the sbrk worked propely
    if (new_block == (void *)-1) {
        return NULL; 
    }
    new_block->size = size;
    return (void *)(new_block + 1);
}

void custom_free(void *pointer) {
    if (pointer == NULL) {
        return;
    }

    memory_block_header *block = (memory_block_header*)pointer - 1;

    block->next = free_list;
    free_list = block;
    fuse_free_list();
    // trim_heap();
    
}

void *custom_realloc(void *pointer, size_t size) {

    if (pointer == NULL) {
        if (size > 0) {
            return custom_malloc(size);
        } else {
            return NULL;
        }
    }
    if (size == 0){
        custom_free(pointer);
        return NULL;
    }


    memory_block_header *old_block = (memory_block_header*)pointer - 1;
    if (old_block->size >= size) {
        // If the current block is already big enough, return the same pointer
        return pointer;
    } 
    
    // We compute the end of the memory block
    char *old_end = (char *)(old_block + 1) + old_block->size;
    // Refuse the free_list just in case
    fuse_free_list();
    // We compute the size to look for in the free list
    size_t additional_size = size - old_block->size;

    memory_block_header *block = free_list;
    while (block!=NULL) {
        if((char *)block == old_end && block->size >= additional_size) { // if the block follows the previous block and has enough space
            remove_element_from_free_list(block);
            old_block->size += block->size + sizeof(memory_block_header);
            return (void *)(old_block + 1);
        }
        block = block->next;
    }

    if (old_end == sbrk(0)) { // Check if the old block is the last block in the heap
        // Expand the heap
        if (sbrk(additional_size) == (void *)-1) {
            return NULL;
        }
        old_block->size = size;
        return pointer;
    } 

    void *new_block = custom_malloc(size);
    if (new_block == NULL) {
        return NULL;
    }
    size_t size_to_copy = old_block->size < size ? old_block->size : size;
    memcpy(new_block, pointer, size_to_copy);
    custom_free(pointer);
    return new_block;
}

void *custom_calloc(size_t n, size_t size) {
    if (n<=0 || size<=0) return NULL;

    size_t total_size = n * size;

    if (size != 0 && total_size / size != n) {
        return NULL; // Multiplication overflowed because total_size is bigger than the max size of a size_t
    }

    void *pointer = custom_malloc(total_size);
    if (pointer == NULL) return NULL;

    memset(pointer, 0, total_size);
    return pointer;

}

int main(int agrc, char *argv[]) {

    int i, n;
    int *a;

    printf("Number of elements to be entered:");
    scanf("%d",&n);

    a = (int*)custom_calloc(n, sizeof(int));
    printf("Enter %d numbers:\n",n);
    for( i=0 ; i < n ; i++ ) {
       scanf("%d",&a[i]);
    }

    printf("The numbers entered are: ");
    for( i=0 ; i < n ; i++ ) {
      printf("%d ",a[i]);
    }
    custom_free( a );
   
    return(0);


}