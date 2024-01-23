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

int main(int agrc, char *argv[]) {
    int *counter = (int*) custom_malloc(sizeof(int));
    printf("%i\n", (int) counter);
    printf("%i\n", *counter);
}