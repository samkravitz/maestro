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
	(void) size;
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
