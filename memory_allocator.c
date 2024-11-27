#include <assert.h>
#include <stdio.h>
#include <unistd.h> // For sbrk

#include "memory_manager.h"


int main(int argc, char *argv[])
{
    printf("Hello, World!\n");

    memory_manager_print_heap();
    void *ptr1 = memory_manager_malloc(100);
    memory_manager_print_heap();

    printf("ptr1 = %p\n", ptr1);
    assert(ptr1 != NULL);

    memory_manager_free(ptr1);
    memory_manager_print_heap();

    

    return 0;
}