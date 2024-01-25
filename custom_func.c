#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct memory_block_header {
    size_t size;
    struct memory_block_header *next;
} memory_block_header;


memory_block_header *free_list = NULL;

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
    if (block == (void *)-1) {
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
}

int main(int agrc, char *argv[]) {

    int *counter = (int*) custom_malloc(sizeof(4));
    printf("Memory allocated at address %p with a value of %i\n", counter, *counter);

    printf("Currently no element of free_list, so its value is pointing at NULL : %p\n", free_list);
    custom_free(counter);
    printf("We have freed the memory, so free_list includes now a pointer to %p (counter address minus the header size)\n", free_list);

    int *counter2 = (int*) custom_malloc(sizeof(int));
    printf("We created a new memory of the same size as the freed one, so it should take the freed address\n");
    printf("Memory allocated at address %p with a value of %i\n", counter2, *counter2);


}