/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/malloc.c
 * DATE: April 27th, 2022
 * DESCRIPTION: userspace malloc modified from Doug Lea's malloc:
 * 	http://gee.cs.oswego.edu/dl/html/malloc.html
 */

#include "malloc.h"

#include <stddef.h>
#include "string.h"

// rounds an number x up to the nearest multiple of 8
#define round8(x) ((x + 7) & ~0x7);

static bool initialized = false;

header freelistSentinels[N_LISTS];
header *lastFencePost;
void *base;

static inline unsigned int get_freelist_index(size_t allocation_size);
static inline unsigned int get_freelist_index_from_header(header *h);
static inline header *get_allocable_block(size_t request_size);
static inline bool is_freelist_empty(unsigned int idx);
static inline void remove_from_freelist(header *h);
static inline void insert_into_freelist(header *h);

// Helper functions for manipulating pointers to headers
static inline header *get_header_from_offset(void *ptr, ptrdiff_t off);
static inline header *get_left_header(header *h);
static inline header *ptr_to_header(void *p);

// Helper functions for allocating more memory from the OS
static inline void initialize_fencepost(header *fp, size_t left_size);
static inline void insert_fenceposts(void *raw_mem, size_t size);
static header *allocate_chunk(size_t size);

// Helper functions for freeing a block
static inline void deallocate_object(void *p);

// Helper functions for allocating a block
static inline header *allocate_object(size_t raw_size);

static void init();

static inline unsigned int get_freelist_index(size_t allocation_size)
{
	if (allocation_size > (N_LISTS - 1) * 8)
		return N_LISTS - 1;
	
	return (allocation_size / 8) - 1;
}

static inline unsigned int get_freelist_index_from_header(header *h)
{
	return get_freelist_index(get_block_size(h) - ALLOC_HEADER_SIZE);
}

/**
 * @brief Helper function to retrieve a header pointer from a pointer and an
 *        offset
 * @param ptr base pointer
 * @param off number of bytes from base pointer where header is located
 * @return a pointer to a header offset bytes from pointer
 */
static inline header *get_header_from_offset(void *ptr, ptrdiff_t off)
{
	return (header *) ((char *) ptr + off);
}

/**
 * @brief Helper function to get the header to the right of a given header
 * @param h original header
 * @return header to the right of h
 */
header *get_right_header(header *h)
{
	return get_header_from_offset(h, get_block_size(h));
}

/**
 * @brief Helper function to get the header to the left of a given header
 * @param h original header
 * @return header to the right of h
 */
inline static header *get_left_header(header *h)
{
	return get_header_from_offset(h, -h->left_size);
}

/**
 * @brief Fenceposts are marked as always allocated and may need to have
 * a left object size to ensure coalescing happens properly
 *
 * @param fp a pointer to the header being used as a fencepost
 * @param left_size the size of the object to the left of the fencepost
 */
inline static void initialize_fencepost(header *fp, size_t left_size)
{
	set_block_state(fp, FENCEPOST);
	set_block_size(fp, ALLOC_HEADER_SIZE);
	fp->left_size = left_size;
}

/**
 * @brief given a chunk of memory insert fenceposts at the left and
 * right boundaries of the block to prevent coalescing outside of the
 * block
 *
 * @param raw_mem a void pointer to the memory chunk to initialize
 * @param size the size of the allocated chunk
 */
inline static void insert_fenceposts(void *raw_mem, size_t size)
{
	// Convert to char * before performing operations
	char *mem             = (char *) raw_mem;

	// Insert a fencepost at the left edge of the block
	header *leftFencePost = (header *) mem;
	initialize_fencepost(leftFencePost, ALLOC_HEADER_SIZE);

	// Insert a fencepost at the right edge of the block
	header *rightFencePost = get_header_from_offset(mem, size - ALLOC_HEADER_SIZE);
	initialize_fencepost(rightFencePost, size - 2 * ALLOC_HEADER_SIZE);
}

/**
 * @brief Allocate another chunk from the OS and prepare to insert it
 * into the free list
 * @param size The size to allocate from the OS
 * @return A pointer to the allocable block in the chunk (just after the
 * first fencpost)
 */
static header *allocate_chunk(size_t size)
{
	void *mem = sbrk(size);

	insert_fenceposts(mem, size);
	header *hdr = (header *) ((char *) mem + ALLOC_HEADER_SIZE);
	set_block_state(hdr, UNALLOCATED);
	set_block_size(hdr, size - 2 * ALLOC_HEADER_SIZE);
	hdr->left_size = ALLOC_HEADER_SIZE;
	return hdr;
}

static inline header *get_allocable_block(size_t request_size)
{
	size_t allocable_size = request_size - ALLOC_HEADER_SIZE;
	int index = get_freelist_index(allocable_size);
	while (is_freelist_empty(index) && index < N_LISTS - 1)
		index++;
	
	if (index != N_LISTS - 1)
		return freelistSentinels[index].next;
	
	header *sentinal = &freelistSentinels[N_LISTS - 1];
	header *block = sentinal->next;
	while (block != sentinal)
	{
		//printf("hi\n");
		if (get_block_state(block) == UNALLOCATED && get_block_size(block) >= request_size)
			return block;
		block = block->next;
	}

	return NULL;
}

static inline bool is_freelist_empty(unsigned int idx)
{
	//assert(idx > 0 && idx < N_LISTS);
	header *sentinal = &freelistSentinels[idx];
	return sentinal == sentinal->next;
}

static inline void remove_from_freelist(header *h)
{
	h->next->prev = h->prev;
    h->prev->next = h->next;
}

static inline void insert_into_freelist(header *h)
{
	//assert(get_block_state(h) == UNALLOCATED);

	int index = get_freelist_index_from_header(h);
	header *sentinal = &freelistSentinels[index];
	if (is_freelist_empty(index))
	{
		sentinal->prev = h;
		sentinal->next = h;
		h->prev = sentinal;
		h->next = sentinal;
    }
	
	else
	{
		h->next = sentinal->next;
		sentinal->next->prev = h;
		sentinal->next = h;
		h->prev = sentinal;
    }
}

/**
 * @brief Helper allocate an object given a raw request size from the user
 * @param size number of bytes the user needs
 * @return A block satisfying the user's request
 */
static inline header *allocate_object(size_t size)
{
	if (size == 0)
		return NULL;

	size_t request_size = round8(size + ALLOC_HEADER_SIZE);
	if (request_size < sizeof(header))
		request_size = sizeof(header);
	
	header *block = get_allocable_block(request_size);
	if (!block)
	{
		header *chunk = allocate_chunk(ARENA_SIZE);
		header *first_fencepost = get_left_header(chunk);
		header *last_fencepost = get_right_header(chunk);
		header *last_allocable_block = get_left_header(lastFencePost);
		int old_fl_idx = get_freelist_index_from_header(last_allocable_block);
		if (get_right_header(lastFencePost) == first_fencepost)
		{
			if (get_block_state(last_allocable_block) == UNALLOCATED)
			{
				int old_fl_index = get_freelist_index_from_header(last_allocable_block);
				set_block_size(last_allocable_block, get_block_size(last_allocable_block) + 2 * ALLOC_HEADER_SIZE + get_block_size(chunk));
				last_fencepost->left_size = get_block_size(last_allocable_block);
				if (old_fl_idx != N_LISTS - 1 && (old_fl_idx != get_freelist_index_from_header(last_allocable_block)))
				{
					remove_from_freelist(last_allocable_block);
					insert_into_freelist(last_allocable_block);
				}
			}

			else if (get_block_state(last_allocable_block) == ALLOCATED)
			{
				set_block_size_and_state(lastFencePost, get_block_size(chunk) + 2 * ALLOC_HEADER_SIZE, UNALLOCATED);
				lastFencePost->left_size = get_block_size(last_allocable_block);
				last_fencepost->left_size = get_block_size(lastFencePost);
				insert_into_freelist(lastFencePost);
			}
		}

		else
		{
			insert_into_freelist(chunk);
		}

		lastFencePost = last_fencepost;
		return allocate_object(size);
	}

	// determine if block can be split into 2 allocable blocks
	size_t block_size = get_block_size(block);
	size_t rem = block_size - request_size;

	// block can be split
	if (rem >= sizeof(header))
	{
		header *left = block;
		size_t left_size = block_size - request_size;
		set_block_size(left, left_size);

		header *right = get_header_from_offset(left, left_size);
		set_block_size(right, request_size);
		right->left_size = left_size;
		get_right_header(right)->left_size = request_size;

		if (get_freelist_index(block_size) != get_freelist_index(left_size))
		{
			remove_from_freelist(left);
			insert_into_freelist(left);
		}

		set_block_state(right, ALLOCATED);
		return (header *) right->data;
	}

	remove_from_freelist(block);
	set_block_state(block, ALLOCATED);
	return (header *) block->data;	
}

/**
 * @brief Helper to get the header from a pointer allocated with malloc
 * @param p pointer to the data region of the block
 * @return A pointer to the header of the block
 */
static inline header *ptr_to_header(void *p)
{
	return (header *) ((char *) p - ALLOC_HEADER_SIZE);    // sizeof(header));
}

/**
 * @brief Helper to manage deallocation of a pointer returned by the user
 * @param p The pointer returned to the user by a call to malloc
 */
static inline void deallocate_object(void *p)
{
	if (!p)
		return;
	
	// header corresponding to the freeing address
	struct header *h = (struct header *) (p - ALLOC_HEADER_SIZE);

	// check for double free
	if (get_block_state(h) != ALLOCATED)
	{
		//printf("Double Free Detected\n");
		//assert(false);
	}

	// get left and right headers to check their allocation status
	struct header *left = get_left_header(h);
	struct header *right = get_right_header(h);
	set_block_state(h, UNALLOCATED);

	// case 1 - neither the right nor the left blocks are unallocated
	if (get_block_state(left) != UNALLOCATED && get_block_state(right) != UNALLOCATED)
	{
		insert_into_freelist(h);
	}

	// case 2 - Only the right block is unallocated
	// coalece right block and h together
	else if (get_block_state(left) != UNALLOCATED && get_block_state(right) == UNALLOCATED)
	{
		int old_fl_index = get_freelist_index_from_header(right);
		set_block_size(h, get_block_size(h) + get_block_size(right));
		if (old_fl_index != get_freelist_index_from_header(h))
		{
			remove_from_freelist(right);
			insert_into_freelist(h);
		}

		else
		{
			right->next->prev = h;
			right->prev->next = h;
		}

		get_right_header(h)->left_size = get_block_size(h);
	}

	// case 3 - Only the left block is unallocated
	// coalece left block and h together
	else if (get_block_state(left) == UNALLOCATED && get_block_state(right) != UNALLOCATED)
	{
		int old_fl_index = get_freelist_index_from_header(left);
		set_block_size(left, get_block_size(left) + get_block_size(h));
		if (old_fl_index != get_freelist_index_from_header(left))
		{
			remove_from_freelist(left);
			insert_into_freelist(left);
		}

		get_right_header(left)->left_size = get_block_size(left);
	}

	// case 4 - both left and right blocks are unallocated
	// coalesce left, h, and right together
	else if (get_block_state(left) == UNALLOCATED && get_block_state(right) == UNALLOCATED)
	{
		int old_fl_index = get_freelist_index_from_header(left);
		set_block_size(left, get_block_size(left) + get_block_size(h) + get_block_size(right));
		if (old_fl_index != get_freelist_index_from_header(left))
		{
			//printf("bing bong\n");
			remove_from_freelist(left);
			insert_into_freelist(left);
		}

		right->prev->next = right->next;
		right->next->prev = left;
		get_right_header(left)->left_size = get_block_size(left);
	}
}

/**
 * @brief Initialize mutex lock and prepare an initial chunk of memory for
 * allocation
 */
static void init()
{
	// Allocate the first chunk from the OS
	header *block         = allocate_chunk(ARENA_SIZE);

	header *prevFencePost = get_header_from_offset(block, -ALLOC_HEADER_SIZE);

	lastFencePost = get_header_from_offset(block, get_block_size(block));

	// Set the base pointer to the beginning of the first fencepost in the first
	// chunk from the OS
	base          = ((char *) block) - ALLOC_HEADER_SIZE;    // sizeof(header);

	// Initialize freelist sentinels
	for (int i = 0; i < N_LISTS; i++)
	{
		header *freelist = &freelistSentinels[i];
		freelist->next   = freelist;
		freelist->prev   = freelist;
	}

	// Insert first chunk into the free list
	header *freelist = &freelistSentinels[N_LISTS - 1];
	freelist->next   = block;
	freelist->prev   = block;
	block->next      = freelist;
	block->prev      = freelist;
}

/*
 * External interface
 */
void *malloc(size_t size)
{
	if (!initialized)
	{
		init();
		initialized = true;
	}

	header *hdr = allocate_object(size);
	return hdr;
}

void *calloc(size_t nmemb, size_t size)
{
	return memset(malloc(size * nmemb), 0, size * nmemb);
}

void *realloc(void *ptr, size_t size)
{
	void *mem = malloc(size);
	memcpy(mem, ptr, size);
	free(ptr);
	return mem;
}

void free(void *p)
{
	deallocate_object(p);
}