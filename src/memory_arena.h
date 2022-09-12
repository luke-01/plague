#ifndef PLAGUE_MEMORY_ARENA_H
#define PLAGUE_MEMORY_ARENA_H

#include <stddef.h>

typedef struct plg_memory_arena_t {
	void*  data;
	size_t size;
	size_t used;
} plg_memory_arena_t;

void* plg_arena_push(plg_memory_arena_t* arena, size_t size);
void  plg_arena_reset(plg_memory_arena_t* arena);

#endif // PLAGUE_MEMORY_ARENA_H
