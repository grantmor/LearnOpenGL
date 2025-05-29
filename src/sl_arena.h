#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sl_type.h"

#pragma once

typedef struct {
	u8* buffer;
	usize buffer_length;
	usize prev_offset;
	usize cur_offset;
} Arena;

bool is_power_of_two(uptr p)
{
	return (p & (p-1)) == 0;
}

uptr align_forward(uptr start, uptr aligned)
{
	// check to make sure this isn't a bug
	uptr p, a, mod;

	p = start;
	a = (uptr) aligned;

	assert(is_power_of_two(aligned));
	mod = p % a;

	if (mod != 0)
	{
		p += a-mod;
	}

	return p;
}

void* arena_alloc_align(Arena* arena, usize size, usize align)
{
	// Align curOffset to alignment
	uptr cur_ptr = (uptr) arena->buffer + (uptr) arena->cur_offset;
	uptr offset = align_forward(cur_ptr, align);
	offset -= (uptr) arena->buffer;

	// Check to see if backing memory has space left
	if (offset + size <= arena->buffer_length)
	{
		arena->prev_offset = offset;
		arena->cur_offset = arena->cur_offset + size;

		void* new_start = &arena->buffer[offset];
		memset(new_start, 0, size);

		return new_start;
	}

	// Return NULL if arena is out of memory
	return NULL;

}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

void* arena_alloc(Arena* arena, usize size)
{
	return arena_alloc_align(arena, size, DEFAULT_ALIGNMENT);
}

void arena_init(Arena* arena, u8* backing_buffer, usize buffer_length)
{
	arena->buffer = backing_buffer;
	arena->buffer_length = buffer_length;
	arena->cur_offset = 0;
	arena->prev_offset = 0;
}

void arena_reset(Arena* arena)
{
	arena->cur_offset = 0;
	arena->prev_offset = 0;
}
