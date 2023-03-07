/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Authors: Noah Bennett & Sam Kravitz
 *
 * FILE: kmalloc.h
 * DATE: March 14th, 2022
 * DESCRIPTION: Implentation of a kernel heap
 */
#ifndef KMALLOC_H
#define KMALLOC_H

#include <maestro.h>

// minimum size in bytes that can be kmalloc'd
#define MIN_ALLOCATION    8

// size of the header for an allocated block
#define ALLOC_HEADER_SIZE (sizeof(struct header) - (2 * sizeof(struct header *)))

/**
 * @brief represents the allocation state of an object
 */
enum state
{
	UNALLOCATED = 0,
	ALLOCATED   = 1,
	SENTINAL    = 2,
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
struct header
{
	// encoding of the object's size & state
	size_t size_state;

	// size in bytes of the object to the left of this
	size_t left_size;
	union
	{
		// used when the object is free
		struct
		{
			struct header *next;    // ptr to next unallocated object
			struct header *prev;    // ptr to previous unallocated object
		};

		// used when the object is allocated
		u8 data[0];
	};
};

// helper functions to work with headers
static inline size_t get_block_size(struct header *h)
{
	return h->size_state & ~0x3;
}

static inline enum state get_block_state(struct header *h)
{
	return (enum state) (h->size_state & 0x3);
}

static inline void set_block_size(struct header *h, size_t size)
{
	h->size_state = size | (h->size_state & 0x3);
}

static inline void set_block_state(struct header *h, enum state s)
{
	h->size_state = (h->size_state & ~0x3) | s;
}

/**
 * @brief get a header pointer from a pointer and an offset
 * @param ptr base pointer
 * @param off number of bytes from base pointer where header is located
 * @return a pointer to a header offset bytes from pointer
 */
static inline struct header *get_header_from_offset(void *ptr, ptrdiff_t off)
{
	return (struct header *) ((char *) ptr + off);
}

/**
 * @brief gets the header to the right of a given header
 * @param h original header
 * @return header to the right of h
 */
static inline struct header *get_right_header(struct header *h)
{
	return get_header_from_offset(h, get_block_size(h));
}

/**
 * @brief gets the header to the left of a given header
 * @param h original header
 * @return header to the left of h
 */
inline static struct header *get_left_header(struct header *h)
{
	return get_header_from_offset(h, -h->left_size);
}

void kmalloc_init(void *, size_t);

void *kmalloc(size_t);
void *kmalloc_a(size_t, size_t);
void kfree(void *);

void print_heap();
void print_freelist();

#endif    // KMALLOC_H
