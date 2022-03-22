/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Authors: Noah Bennett & Sam Kravitz
 *
 * FILE: kmalloc.c
 * DATE: March 14th, 2022
 * DESCRIPTION: Implentation of a kernel heap
 */
#include <kmalloc.h>
#include <kprintf.h>

// freelist sentinal - denotes the head of the doubly-linked freelist
static struct header *sentinal;

// pointers to the base of the heap, and current top
void *base, *heap;

// rounds an number x up to the nearest multiple of 8
#define round8(x) ((x + 7) & ~0x7);

void kmalloc_init(void *p, size_t s)
{
    base = p;
	heap = base + s;

	// initialize the freelist
	sentinal = (struct header *) p;
	set_block_size(sentinal, sizeof(struct header));
	set_block_state(sentinal, SENTINAL);

	// first unallocated object
	struct header *h = base + sizeof(struct header);
	set_block_size(h, s - sizeof(struct header));
	set_block_state(h, UNALLOCATED);

	sentinal->next = h;
	sentinal->prev = h;
	sentinal->left_size = 0;

	h->next = sentinal;
	h->prev = sentinal;
	h->left_size = get_block_size(sentinal);

	(void) print_heap;
	(void) print_freelist;
}

/**
 * @brief allocates memory on the kernel's heap
 * @param size the size in bytes of the request
 * @return pointer to the first usable data byte of the request
 */
void *kmalloc(size_t size)
{
	if (size == 0)
		return NULL;

	// total size of object in bytes, includes user request and metadata
	size_t total_size = size;

	if (total_size < MIN_ALLOCATION)
		total_size = MIN_ALLOCATION;

	total_size = round8(total_size);
	total_size += ALLOC_HEADER_SIZE;

	// loop through freelist to find a block big enough for request
	struct header *block = sentinal->next;
	while (block != sentinal)
	{
		// block is big enough for our request
		if (get_block_size(block) >= total_size)
		{
			// get remaining size of this block after request has been served
			size_t rem = get_block_size(block) - total_size;

			// block can be split into 2 allocable blocks
			if (rem >= MIN_ALLOCATION + ALLOC_HEADER_SIZE)
			{
				// split the block into a right and left block
				// return to the user the block higher in memory (right block)

				// left block
				struct header *left = block;
				set_block_size(left, rem);

				// right block (the one being returned to the user)
				struct header *right = get_right_header(block);
				set_block_size(right, total_size);
				set_block_state(right, ALLOCATED);
				right->left_size = get_block_size(left);

				// on most splits, the block to the right of the right block
				// will need to have its left_size adjusted
				struct header *right_right = get_right_header(right);
				if ((uintptr_t) right_right < (uintptr_t) heap)
					right_right->left_size = get_block_size(right);

				return (void *) right->data;
			}

			// block cannot be split into 2 allocable blocks
			// simply remove the block from the freelist
			else
			{
				block->prev->next = block->next;
				block->next->prev = block->prev;
				set_block_state(block, ALLOCATED);

				return (void *) block->data;
			}
		}

		block = block->next;
	}

	// no block in freelist was big enough to accomodate our request,
	// so we have to expand the heap
	kprintf("Time to expand the heap!\n");

	return NULL;
}

/**
 * @brief deallocates memory from the kernel's heap
 * @param p pointer to user's data that was returned by kmalloc
 */
void kfree(void *p)
{
	(void) p;
}

static const char *state_strings[] = {
	"UNALLOCATED",
	"ALLOCATED",
	"SENTINAL",
};

void print_heap()
{
	kprintf("\tHEAP: \n");
	struct header *h = base;

	while ((uintptr_t) h < (uintptr_t) heap)
	{
		kprintf("addr: 0x%x\n", h);
		kprintf("size: %d (%xh)\n", get_block_size(h), get_block_size(h));
		kprintf("state: %s\n", state_strings[get_block_state(h)]);
		kprintf("left size: %d (%xh)\n\n", h->left_size, h->left_size);

		h = get_right_header(h);
	}
}

void print_freelist()
{
	kprintf("\tFREELIST: \n");
	struct header *freelist = sentinal;

	do
	{
		kprintf("addr: 0x%x\n", freelist);
		kprintf("size: %d (%xh)\n", get_block_size(freelist), get_block_size(freelist));
		kprintf("prev: 0x%x\n", freelist->prev);
		kprintf("next: 0x%x\n\n", freelist->next);

		freelist = freelist->next;
	} while (freelist != sentinal);
}
