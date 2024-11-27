memory_allocator : memory_allocator.c memory_manager.c memory_manager.h
	gcc -o memory_allocator memory_allocator.c memory_manager.c