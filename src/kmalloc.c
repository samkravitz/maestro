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
#include <vmm.h>

// freelist sentinal - denotes the head of the doubly-linked freelist
static struct header *sentinal;

// pointers to the base of the heap, and current top
void *base, *heap;

// rounds an number x up to the nearest multiple of 8
#define round8(x) ((x + 7) & ~0x7);

static void insert_into_freelist(struct header *);

void kmalloc_init(void *p, size_t s)
{
	base = p;
	heap = p;

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

	void *ptr = heap;
	heap += round8(size);
	return ptr;
}

void *kmalloc_a(size_t size, size_t alignment)
{
	while ((uintptr_t) heap % alignment != 0)
		heap += 1;

	return kmalloc(size);
}

/**
 * @brief deallocates memory from the kernel's heap
 * @param p pointer to user's data that was returned by kmalloc
 */
void kfree(void *p) { }

/**
 * @brief inserts a header in the front of a freelist
 * @param h header to insert
 */
static void insert_into_freelist(struct header *h)
{
	// freelist is empty
	if (sentinal->next == sentinal)
	{
		sentinal->prev = h;
		sentinal->next = h;
		h->prev        = sentinal;
		h->next        = sentinal;
	}

	// freelist is not empty
	else
	{
		h->next              = sentinal->next;
		h->prev              = sentinal;
		sentinal->next->prev = h;
		sentinal->next       = h;
	}
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
