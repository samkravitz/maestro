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

#include "string.h"

// kernel page directory
extern struct pde kpage_dir[NUM_TABLE_ENTRIES];

// kernel page table
extern struct pte kpage_table[NUM_TABLE_ENTRIES];

// identity page table
extern struct pde ident_page_table[NUM_TABLE_ENTRIES];

extern u32 start_phys, start;

// converts virtual address addr to a physical address
#define VIRT_TO_PHYS(addr) ((u32) &start_phys + (u32) addr - (u32) &start)

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
	// copy tables from where bootloader placed them to safe kernel memory
	memcpy(kpage_dir,        KPAGE_DIR_BASE,   PAGE_DIR_SIZE);
	memcpy(kpage_table,      KPAGE_TABLE_BASE, PAGE_DIR_SIZE);
	memcpy(ident_page_table, IDENT_TABLE_BASE, PAGE_DIR_SIZE);

	// set kernel pde for identity page table and kernel page table to new addr
	int i = (u32) &start / 0x400000; // index into kernel page directory that maps the kernel page table
	kpage_dir[i].addr = VIRT_TO_PHYS(kpage_table) >> 12;
	kpage_dir[0].addr = VIRT_TO_PHYS(ident_page_table) >> 12;

	// mark physical addresses 0x0000-0x1000 as not present
	// this will make all null pointer dereferences page fault
	ident_page_table[0].present = 0;

	// move physical address of kernel page directory to cr3
	asm("mov %0, %%cr3" :: "r"(VIRT_TO_PHYS(kpage_dir)));
}
