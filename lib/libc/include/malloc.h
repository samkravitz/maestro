/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/malloc.h
 * DATE: April 27th, 2022
 * DESCRIPTION: userspace malloc modified from Doug Lea's malloc:
 * 	http://gee.cs.oswego.edu/dl/html/malloc.html
 */

#ifndef MALLOC_H
#define MALLOC_H

#include <stdbool.h>
#include <stddef.h>

// size in bytes of chunks requested from OS
#define ARENA_SIZE 4096

// number of freelists
#define N_LISTS 59

// size of the header for an allocated block
#define ALLOC_HEADER_SIZE (sizeof(header) - (2 * sizeof(header *)))

// minimum size in bytes the allocator will service
#define MIN_ALLOCATION    8

/**
 * @brief represents the allocation state of an object
 */
enum state
{
	UNALLOCATED = 0,
	ALLOCATED   = 1,
	FENCEPOST   = 2,
};

/**
 * @brief contains metadata about a heap object
 * 
 * objects must be a multiple of 8 bytes, which means the size of the object
 * will always end in 0b00. Therefore we can use these bottom 2 bits
 * to encode some information about the allocation state of the object.
 * 
 * additionally, we use a union because different metadata is needed depending on whether an
 * object is free or allocated. When allocated, data points to the first byte of data of the
 * object (i.e, the ptr returned to the user by malloc). When unallocated, we instead keep 
 * pointers to the next and previous object in the free list
 */
typedef struct header
{
	size_t size_and_state;
	size_t left_size;
	union
	{
		// Used when the object is free
		struct
		{
			struct header *next;
			struct header *prev;
		};
		// Used when the object is allocated
		char data[0];
	};
} header;

// Helper functions for getting and storing size and state from header
// Since the size is a multiple of 8, the last 3 bits are always 0s.
// Therefore we use the 3 lowest bits to store the state of the object.
// This is going to save 8 bytes in all objects.

static inline size_t get_block_size(header *h)
{
	return h->size_and_state & ~0x3;
}

static inline void set_block_size(header *h, size_t size)
{
	h->size_and_state = size | (h->size_and_state & 0x3);
}

static inline enum state get_block_state(header *h)
{
	return (enum state)(h->size_and_state & 0x3);
}

static inline void set_block_state(header *h, enum state s)
{
	h->size_and_state = (h->size_and_state & ~0x3) | s;
}

static inline void set_block_size_and_state(header *h, size_t size, enum state s)
{
	h->size_and_state = (size & ~0x3) | (s & 0x3);
}

// Malloc interface
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *p);

// Helper to find a block's right neighbor
header *get_right_header(header *h);

#endif    // MALLOC_H
