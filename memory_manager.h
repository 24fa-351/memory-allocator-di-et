#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h> // For size_t

void *memory_manager_malloc(size_t size);
void memory_manager_free(void *ptr);
void *memory_manager_realloc(void *ptr, size_t size);

void memory_manager_print_heap();


#endif // MEMORY_MANAGER_H
