#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sbrk
#include <string.h> // For memset

#include "memory_manager.h"

#define LIST_SIZE 1000
#define MIN_BLOCK_SIZE 4096

typedef struct block
{
    size_t size;
    char *address;
    int is_free;
} block_t;

block_t blocks[LIST_SIZE];

int blocks_in_use = 0;

void *malloc(size_t requested_size)
{
    // look for a block in blocks that is free and is bigger than requested siz
    for (int ix = 0; ix < blocks_in_use; ix++)
    {
        if (blocks[ix].is_free && blocks[ix].size >= requested_size)
        {
            block_t *huge_block = &blocks[ix];
            // if found, return the address of the block, marked its not free(check)
            size_t rest = huge_block->size - requested_size;
            // if its to big split it and put the rest as a free block(check)
            if (rest > 0)
            {
                // save the rest as a new free block
                blocks[blocks_in_use].size = rest; // size of the free block
                blocks[blocks_in_use].address = &huge_block->address[requested_size];
                blocks[blocks_in_use].is_free = 1; // is.free is free

                blocks_in_use++;

                huge_block->size = requested_size;
            }
            huge_block->is_free = 0;
            memory_manager_print_heap();
            return huge_block->address;
        }
    }

    // if not found, use sbrk to get more memory and put it in a new block and return the apporpriate amount of memory
    int allocated_size = requested_size;
    if (allocated_size < MIN_BLOCK_SIZE)
    {
        allocated_size = MIN_BLOCK_SIZE;
    }
    void *allocated_memory_block = sbrk(allocated_size);

    blocks[blocks_in_use].size = requested_size;
    blocks[blocks_in_use].address = allocated_memory_block;
    blocks[blocks_in_use].is_free = 0; // is.free is in use
    blocks_in_use++;

    if (allocated_size > requested_size)
    {
        blocks[blocks_in_use].size = allocated_size - requested_size;
        blocks[blocks_in_use].address = allocated_memory_block + requested_size;
        blocks[blocks_in_use].is_free = 1;
        blocks_in_use++;
    }
    return allocated_memory_block;
}

void free(void *ptr)
{
    for (int ix = 0; ix < blocks_in_use; ix++)
    {
        if (blocks[ix].address == ptr)
        {
            blocks[ix].is_free = 1;
            return;
        }
    }
    printf("Error in memory_manager_free: block not found (%p) memory block\n", ptr);
    memory_manager_print_heap();
}

void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
    {
        perror("ptr is NULL");
        return malloc(size);
    }
    if (size == 0)
    {
        perror("size is 0");
        free(ptr);
        return NULL;
    }

    for (int ix = 0; ix < blocks_in_use; ix++)
    {
        block_t *old_block = &blocks[ix];
        if (old_block->address != ptr)
            continue;

        if (old_block->size >= size)
        {
            return ptr;
        }

        void *new_ptr = malloc(size);
        if (new_ptr == NULL)
        {
            return NULL;
        }
        memcpy(new_ptr, ptr, old_block->size);
        free(ptr);

        return new_ptr;
    }
    memory_manager_print_heap();
    return NULL;
}

void memory_manager_print_heap()
{
    for (int ix = 0; ix < blocks_in_use; ix++)
    {
        printf("Block %3d: size=%5ld, address=%p, is_free=%d\n", ix, blocks[ix].size, blocks[ix].address, blocks[ix].is_free);
    }
}
