/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: vmm.c
 * DATE: March 15th, 2022
 * DESCRIPTION: virtual memory manager
 */

#include <vmm.h>

#include <intr.h>
#include <kprintf.h>
#include <kmalloc.h>
#include <pmm.h>

#include "stdio.h"
#include "string.h"

// kernel page directory
extern struct pde kpage_dir[NUM_TABLE_ENTRIES];

// kernel page table
extern struct pte kpage_table[NUM_TABLE_ENTRIES];

// identity page table
extern struct pde ident_page_table[NUM_TABLE_ENTRIES];

extern u32 start_phys, start;

// pointer to heap, defined in kmalloc.c
extern void *heap;

// converts virtual address addr to a physical address
#define VIRT_TO_PHYS(addr) ((u32) &start_phys + (u32) addr - (u32) &start)

static void page_fault();

/**
 * the bootloader kept data structures for initializing paging in the first ~10K of memory.
 * once our pmm is initialized, that region of memory will be marked as available
 * and could be overridden at any point, so we need to copy those data structures
 * to kernel memory so we know it won't be overriden.
 * 
 * the 3 data structures we have to save are the kernel's page directory,
 * the page table that identity maps the first 1M of memory,
 * and the page table that maps the first 4M of the kernel.
 */
void vmm_init()
{
	set_vect(14, page_fault);

	// copy tables from where bootloader placed them to safe kernel memory
	memcpy(kpage_dir,        KPAGE_DIR_BASE,   PAGE_DIR_SIZE);
	memcpy(kpage_table,      KPAGE_TABLE_BASE, PAGE_DIR_SIZE);
	memcpy(ident_page_table, IDENT_TABLE_BASE, PAGE_DIR_SIZE);

	// set kernel pde for identity page table and kernel page table to new addr
	int i = (u32) &start / 0x400000; // index into kernel page directory that maps the kernel page table
	kpage_dir[i].addr = VIRT_TO_PHYS(kpage_table) >> 12;
	kpage_dir[0].addr = VIRT_TO_PHYS(ident_page_table) >> 12;
	kpage_dir[i].user = 1;

	// when mapping the kernel to 0xc0000000, the bootloader mapped 4M of memory.
	// However, not all of that is used. The unused pages should be marked as not present.
	// the used pages are the pages used by the kernel and the page(s) used by the physical memory bitmap.

	// map 1 Mb for the heap
	uint heap_pages = 1024 * 1024 / PAGE_SIZE;
	vmm_alloc((uintptr_t) heap, heap_pages);

	// index into page table that maps heap's first page
	i = ((u32) heap - (u32) &start) / PAGE_SIZE;

	for (int j = 0; j <= 1024; j++)
		kpage_table[j].user = 1;

	// start at i + heap_pages because we want to keep the heap's page present
	for (int j = i + heap_pages; j < NUM_TABLE_ENTRIES; j++)
		kpage_table[j].present = 0;

	// mark physical addresses 0x0000-0x1000 as not present
	// this will make all null pointer dereferences page fault
	ident_page_table[0].present = 0;

	// move physical address of kernel page directory to cr3
	asm("mov %0, %%cr3" :: "r"(VIRT_TO_PHYS(kpage_dir)));

	kmalloc_init(heap, 1024 * 1024);
}

/**
 * @brief allocate virtual memory
 * @param virt page aligned virtual address to start mapping from
 * @param count number of pages to allocate
 * @return beginning of mapped memory
 * 
 * @note the granularity of this allocator is the size of a page
 */
void *vmm_alloc(uintptr_t virt, size_t count)
{
	uintptr_t v = virt;
	for (size_t i = 0; i < count; i++)
	{
		// allocate physical memory for this page
		uintptr_t phys = pmm_alloc();

		// figure out which index in the kernel page table the page corresponds to
		int idx = (v - (u32) &start) / PAGE_SIZE;

		struct pte *page = &kpage_table[idx];

		page->present = 1;
		page->addr = phys >> 12;
		v += PAGE_SIZE;
	}

	return (void *) virt;
}

/**
 * @brief page fault handler
 */
static void page_fault()
{
	kprintf("Page fault detected!\n");
	while (1) ;
}
