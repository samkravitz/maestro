#include <kmalloc.h>

#include "string.h"

// start of heap begins at end of maestro image
extern u32 end;
uptr heap        = (uptr) &end;
void *block_head = NULL;

void *brk(int amt)
{
	uptr ptr = heap;
	heap += amt;
	return (void *) ptr;
}

/**
 * set a block of memory as free as in libre
 * @param ptr pointer to the memory block to mark as free
 */
void kfree(void *ptr)
{
	// ignore null pointers :o
	if (!ptr)
		return;

	// get memory block that is being freed
	struct mem_block *freeing_block_ptr = get_block_ptr(ptr);

	// return if block is already free
	if (freeing_block_ptr->free)
		return;

	// assign block as free
	// debug will be 0xBAADF00D if successfully freed
	// the block previous to the block that is being freed
	// is assigned to the next block to prevent 'gaps' in the
	// memory that is available
	freeing_block_ptr->free  = 1;
	freeing_block_ptr->debug = 0xBAADF00D;

	if (freeing_block_ptr->prev)
		freeing_block_ptr->prev->next = freeing_block_ptr->next;
}

/**
 * kernel malloc
 * @param size malloc size in n bytes
 */
void *kmalloc(size_t size)
{
	struct mem_block *i_am;
	// todo: handle alignment with page
	// actually i thhink it works with kmalloca

	// if someone is playing games... return null
	if (size <= 0)
		return NULL;

	if (!block_head)
	{
		// if head is undefined, make request for memory
		// if denied return null, otherwise we know
		// the memory block head is defined
		i_am = request_memory(NULL, size);
		if (!i_am)
			return NULL;
	}

	else
	{
		// keep previous block head, next block head is
		// defined when finding the next free
		struct mem_block *was = block_head;
		i_am                  = find_next_free(&was, size);

		if (!i_am)
		{
			// request more mem if no free blocks were found
			i_am = request_memory(was, size);

			// if request from was was denied return null
			if (!i_am)
				return NULL;
		}

		else
		{
			// successfully found a free block
			i_am->free  = 0;
			i_am->debug = 0xBAD1DEED;
			i_am->prev  = was;
		}
	}

	// return the available memory
	return ++i_am;
}

/**
 * @param size size of malloc in n bytes
 * @param align boolean indicating whether or not the
 * address returned should be aligned to page boundary
 */
void *kmalloca(size_t size)
{
	if (heap & 0xFFFFF000)
	{
		heap &= 0xFFFFF000;

		// if the previous operation rounds down, we don't want to return any memory
		// that may have already been kmalloc'd. so, we'll add a page size to ensure
		// this memory is new.
		heap += _PAGE_SIZE;
	}

	return kmalloc(size);
}

// kmalloc phys - fills the phys pointer with the physical address of the returned memory
void *kmallocp(size_t nbytes, u32 *phys)
{
	uptr ptr = (uptr) kmalloc(nbytes);
	if (phys)
		*phys = ptr;

	return (void *) ptr;
}

/**
 * resize a block of memory
 * @param ptr pointer to the block of memory
 * @param size new size to allocate to
 */
void *krealloc(void *ptr, size_t size)
{
	// if null pointer is passed, realloc functions as malloc
	if (!ptr)
		return kmalloc(size);

	// get the current block
	struct mem_block *my = get_block_ptr(ptr);
	if (my->size > size)
	{
		// decide for caller that there's already enough memory...
		// blocks could split here in the future, thus freeing some
		// of the memory that is unused in this block, but for now
		// that isn't necessary
		return ptr;
	}

	// initialize new block
	void *new_block_ptr = kmalloc(size);

	// failled to malloc
	if (!new_block_ptr)
		return NULL;

	// copy memory from old block to new block, then
	// free the old block
	memcpy(new_block_ptr, ptr, my->size);
	kfree(ptr);
	return new_block_ptr;
}

/**
 * clear an allocation n elements
 * @param n number of elements
 * @param size_el size of each element
 */
void *kcalloc(size_t len, size_t size_el)
{
	size_t size = len * size_el;
	void *ptr;

	// check for overflow
	if (size > heap)
		return NULL;

	// initialize allocation to zero
	ptr = kmalloc(size);
	memset(ptr, 0, size);
	return ptr;
}

/**
 * find the next available memory block, if one exists
 * @param was double pointer to previous memory block
 * @param size size that is currently trying to be malloc'd 
 */
struct mem_block *find_next_free(struct mem_block **was, size_t size)
{
	struct mem_block *i_am = block_head;
	int i                  = 0;
	// iterate through the list until free mem is found
	for (;;)
	{
		if (i_am && i_am->free && i_am->size >= size)
		{
			// if block is not null, and the block is free, and
			// the block is of a valid size, correct block
			// has been found
			break;
		}

		*was = i_am;
		i_am = i_am->next;
		i++;
	}

	// return the found free block
	return i_am;
}

/**
 * ask `brk` for more memory
 * @param was pointer to the previous memory block
 * @param size n bytes to request
 */
struct mem_block *request_memory(struct mem_block *was, size_t size)
{
	struct mem_block *block;
	block               = brk(0);
	void *requested_mem = brk(sizeof(*block) + size);

	// failed to fetch more memory
	if (requested_mem == (void *) -1)
		return NULL;

	// was will always be null on the first request
	// because we're starting with the head of the list
	if (was)
		was->next = block;

	// prepend this block to the head of the heap and
	// append the new tail of the list to this block
	block->next  = NULL;
	block->prev  = was;
	block->size  = size;
	block->free  = 0;
	block->debug = 0xBADDDD1E;
	return block;
}

/**
 * get pointer to the block of memory
 * just does pointer - 1 but is more semantic / easier
 * to read than pointer arithmetic and casting to struct
 * each time
 * @param ptr pointer to get block of memory for
 */
struct mem_block *get_block_ptr(void *ptr)
{
	return ((struct mem_block *) ptr) - 1;
}
