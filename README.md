# Custom Malloc in C

This repository contains a custom implementation of memory allocation functions in C, including `malloc`, `free`, `realloc`, and `calloc`. The implementation focuses on managing memory blocks and optimizing memory usage.

## Getting Started


### Installation

Clone the repository:

```bash
git clone https://github.com/ZilbaM/custom-malloc-c.git
```

### Usage

Compile the c file using GCC :
```bash
    gcc -o yourFileName custom_func.c
```
You can now run the newly created file directly in the terminal :
```bash
    ./yourFileName
```

Precompiled version of the code testing the different functions and features are already available in the repository : 
- `custom_malloc` : shows the use of the `custom_malloc` function.
- `custom_free` : shows the use of the `custom_free` function.
- `custom_calloc` : shows the use of the `custom_calloc` function.
- `custom_realloc` : shows the use of the `custom_realloc` function.
- `sort_free` : shows the use of the `sort_free_list` function, a function that sorts the free_list linked list by pointer addresses, useful in the fusing of adjacent free spaces.
- `fuse_free_list` : shows the use of the `fuse_free_list` function, a function that fuses adjacent memory blocks in the list of free memory blocks. It is called after each deallocation to make sure the list is as compact as possible.
- `brk_error` : I wrote a `trim_free`function that was supposed to remove free memory from the list when its adjacent to the break line and that would change the address of the break line of the heap. It seems however that there is a problem with the `brk` function. This compiled file shows the use of the brk function on the return address of a previous sbrk function used directly, not through the custom malloc. There seems to be some issue with the compiler.


## Function Documentation

### `custom_malloc`

#### `void *custom_malloc(size_t size)`
Allocates a block of memory of a specified size.
- **Arguments**
  - `size`: `size_t` - The size of the memory block, in bytes.
- **Returns**
  - `void*`: A pointer to the allocated memory block if successful, or `NULL` if the allocation fails.

### `custom_free`

#### `void custom_free(void *pointer)`
Frees a block of memory allocated by `custom_malloc`.
- **Arguments**
  - `pointer`: `void*` - A pointer to the memory block to be freed.
- **Returns**
  - None.

### `custom_realloc`

#### `void *custom_realloc(void *pointer, size_t size)`
Reallocates a memory block to a new size.
- **Arguments**
  - `pointer`: `void*` - A pointer to the memory block originally allocated with `custom_malloc`.
  - `size`: `size_t` - The new size for the memory block, in bytes.
- **Returns**
  - `void*`: A pointer to the newly allocated memory block with the requested size, or `NULL` if the reallocation fails.

### `custom_calloc`

#### `void *custom_calloc(size_t n, size_t size)`
Allocates an array of `n` elements, each of a size of `size` bytes, and initializes all bytes to zero.
- **Arguments**
  - `n`: `size_t` - The number of elements.
  - `size`: `size_t` - The size of each element.
- **Returns**
  - `void*`: A pointer to the allocated memory block if successful, or `NULL` if the allocation fails.

### `fuse_free_list`

#### `void fuse_free_list()`
Fuses adjacent free blocks in the free list to form larger continuous blocks.
- **Arguments**
  - None.
- **Returns**
  - None.

### `sort_free_list`

#### `void sort_free_list()`
Sorts the free list based on the memory block addresses.
- **Arguments**
  - None.
- **Returns**
  - None.

### `trim_heap`

#### (Currently Not Working)`void trim_heap()`
Attempts to reduce the size of the heap by releasing the last block in the free list.
- **Arguments**
  - None.
- **Returns**
  - None.


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

