#include "memory_arena.h"
#include <stdalign.h>
#include <stdint.h>

// alignment must be a power of 2
static inline size_t align(size_t size, size_t alignment) {
	return (size + alignment - 1) &~ (alignment - 1);
}

void* plg_arena_push(plg_memory_arena_t* arena, size_t size) {
	size = align(size, alignof(max_align_t));

	if (arena->used + size > arena->size) return NULL;

	void* ptr = (uint8_t*)arena->data + arena->used;
	arena->used += size;
	return ptr;
}

void  plg_arena_reset(plg_memory_arena_t* arena) {
	arena->used = 0;
}
