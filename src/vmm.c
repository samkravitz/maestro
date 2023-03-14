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
#include <proc.h>

#include <stdio.h>
#include <string.h>

extern struct proc nullproc;

extern u32 start_phys, start;

// pointer to heap, defined in kmalloc.c
extern void *heap;

// converts virtual address addr to a physical address
#define VIRT_TO_PHYS(addr) ((u32) &start_phys + (u32) addr - (u32) &start)

u32 *PAGE_DIR = (u32 *) 0xfffff000;
void *PAGE_TABLES = (void *) 0xffc00000;

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

    u32 *kpage_table = (u32 *) pmm_alloc();
    u32 *kpage_dir = (u32 *) pmm_alloc();
    u32 *ident_page_table = (u32 *) pmm_alloc();

    for (int i = 0, phys = (int) &start_phys; i < 1024; i++, phys += PAGE_SIZE)
    {
        kpage_dir[i] = 0;
        kpage_table[i] = phys | PT_PRESENT | PT_WRITABLE;
        ident_page_table[i] = PT_PRESENT | PT_WRITABLE;
    }

    // set kernel pde for identity page table and kernel page table to new addr
	kpage_dir[0] = (uintptr_t) ident_page_table | PT_PRESENT | PT_WRITABLE;
	int i = (u32) &start / 0x400000; // index into kernel page directory that maps the kernel page table
    kpage_dir[i] = (uintptr_t) kpage_table | PT_PRESENT | PT_WRITABLE;

    // identity map final entry of kernel page directory
    kpage_dir[1023] = (uintptr_t) kpage_dir | PT_PRESENT | PT_WRITABLE;

	// move physical address of kernel page directory to cr3
	asm("mov %0, %%cr3" :: "r"(kpage_dir));

    // map physical page of VGA framebuffer
    vmm_map_page(0xb8000, 0xb8000, PT_PRESENT | PT_WRITABLE);

	nullproc.pdir = (uintptr_t) kpage_dir;
	kmalloc_init(heap, 1024 * 1024);
}

uintptr_t vmm_create_address_space()
{
	uintptr_t phys = pmm_alloc();
	struct pde *dir = NULL;

	for (int i = 0; i < NUM_TABLE_ENTRIES; i++)
	{
		//if (kpage_table[i].present == 0)
		//{
		//	kprintf("%d %x\n",i, kpage_dir[i].addr);
		//	kpage_table[i].present = 1;
		//	kpage_table[i].addr = phys >> 12;
		//	dir = (struct pde *) &kpage_table[i];
		//	break;
		//}
	}

	if (dir == NULL)
		kprintf("Error creating address space!\n");
	
	//memcpy(dir, kpage_dir, PAGE_DIR_SIZE);
	return phys;
}

void vmm_map_page(uintptr_t phys, uintptr_t virt, unsigned flags)
{
    unsigned long pdindex = virt >> 22;
    unsigned long ptindex = virt >> 12 & 0x3ff;
    if (!(PAGE_DIR[pdindex] & PT_PRESENT))
    {
        kprintf("%d not present\n", pdindex);
        uintptr_t new_page = pmm_alloc();
        PAGE_DIR[pdindex] = new_page | flags;
    }

    u32 *page_table = PAGE_TABLES + pdindex * PAGE_SIZE;
    page_table[ptindex] = phys | flags;
}

/**
 * @brief page fault handler
 */
static void page_fault()
{
	kprintf("Page fault detected!\n");
	while (1) ;
}
