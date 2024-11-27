#include <stdio.h>
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

void *memory_manager_malloc(size_t requestedSize)
{
    // look for a block in blocks that is free and is bigger than requested size (check)
    for (int ix = 0; ix < blocks_in_use; ix++)
    {
        if (blocks[ix].is_free && blocks[ix].size >= requestedSize)
        {
            // if found, return the address of the block, marked its not free(check)
            size_t rest = blocks[ix].size - requestedSize;
            // if its to big split it and put the rest as a free block(check)
            if (rest > 0)
            {
                blocks[blocks_in_use].size = rest;
                blocks[blocks_in_use].address = &blocks[ix].address[requestedSize];
                blocks[blocks_in_use].is_free = 1;
                blocks_in_use++;
            }
            blocks[ix].is_free = 0;
            return blocks[ix].address;
        }
    }
    // if not found, use sbrk to get more memory and put it in a new block and return the apporpriate amount of memory
    int allocated_size = requestedSize;
    if (allocated_size < MIN_BLOCK_SIZE)
    {
        allocated_size = MIN_BLOCK_SIZE;
    }
    void *allocated_memory_block = sbrk(allocated_size);
    blocks[blocks_in_use].size = requestedSize;
    blocks[blocks_in_use].address = allocated_memory_block;
    blocks[blocks_in_use].is_free = 0; // is.free is in use
    blocks_in_use++;

    if (allocated_size > requestedSize)
    {
        blocks[blocks_in_use].size = allocated_size - requestedSize;
        // blocks[blocks_in_use].address = &allocated_memory_block[requestedSize];

        // Option 1: Simple pointer arithmetic
        blocks[blocks_in_use].address = allocated_memory_block + requestedSize;
        // printf("allocated_memory_block=%p, requestedSize=%ld, blocks[blocks_in_use].address=%p\n", allocated_memory_block, requestedSize, blocks[blocks_in_use].address);

        // Option 2: Cast to char* for explicit pointer arithmetic
        // blocks[blocks_in_use].address = (void *)((char *)allocated_memory_block + requestedSize);

        blocks[blocks_in_use].is_free = 1;
        blocks_in_use++;
    }
    return allocated_memory_block;
}

void memory_manager_free(void *ptr)
{
    // find the block in the list and change it from allocated to free
    for (int ix = 0; ix < blocks_in_use; ix++)
    {
        if (blocks[ix].address == ptr)
        {
            blocks[ix].is_free = 1;
            return;
        }
    }
}

void *memory_manager_realloc(void *ptr, size_t size)
{
    printf("memory_manager_realloc(%p, %ld)\n", ptr, size);
    // function shall deallocate the old memory block, and allocate a new memory block of size bytes

    for(int ix = 0; ix < blocks_in_use; ix++)
    {
        if(blocks[ix].address == ptr)
        {
            if(blocks[ix].size >= size)
            {
                return ptr;
            }
            else
            {
                void *new_ptr = memory_manager_malloc(size);
                if(new_ptr == NULL)
                {
                    return NULL;
                }
                memcpy(new_ptr, ptr, blocks[ix].size);
                memory_manager_free(ptr);
                return new_ptr;
            }
        }
    }




    return NULL;
}

void memory_manager_print_heap()
{
    for (int ix = 0; ix < blocks_in_use; ix++)
    {
        printf("Block %d: size=%ld, address=%p, is_free=%d\n", ix, blocks[ix].size, blocks[ix].address, blocks[ix].is_free);
    }
}
