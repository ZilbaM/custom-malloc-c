#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void *bmalloc(unsigned long size) {
    return sbrk(size);
}

int main(int argc, char *argv[]){
    int *count = bmalloc(sizeof(int));
    printf("%i\n", *count);
}