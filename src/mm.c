/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: mm.c
 * DATE: August 8th, 2021
 * DESCRIPTION: memory management for maestro
 */
#include <mm.h>

#include <intr.h>
#include <kout.h>
#include <kmalloc.h>
#include "string.h"

// macros to manipulate bitmap
#define MMAP_SET(bit)		(meminfo.mmap[bit / 8]  =  (1 << (bit % 8)))
#define MMAP_CLEAR(bit)		(meminfo.mmap[bit / 8] &= ~(1 << (bit % 8)))
#define MMAP_TEST(bit)		(meminfo.mmap[bit / 8] &   (1 << (bit % 8)))

// address of multiboot information struct 
extern uptr mboot_info;

struct meminfo meminfo;

u32 *kpd; // kernel page directory
u32 *cpd; // current page directory
struct pagetab *pagetable;

// page fault handler
void pfault() { }

// gets index of first available frame
int first_free_frame()
{
	for (uint i = 0; i < meminfo.max_blocks / 8; ++i)
	{
		// all 8 bits are set
		if (meminfo.mmap[i] == 0xff)
			continue;
		
		for (int j = 0; j < 8; j++)
		{
			uint bit = i * 8 + j;
			if (MMAP_TEST(bit) == 0)
				return bit;
		}
	}

	return -1;
}

// allocate a free block
void *balloc()
{
	if (meminfo.used_blocks == meminfo.max_blocks)
		return 0;

	int frame;
	if ((frame = first_free_frame()) == -1)
		return 0;
	
	MMAP_SET(frame);

	uptr addr = frame * PAGE_SIZE;
	meminfo.used_blocks++;
	return (void *) addr;
}

// deallocate a block
void bfree(void *block)
{
	uptr addr = (uptr) block;
	int frame = addr / PAGE_SIZE;
	MMAP_CLEAR(frame);
	meminfo.used_blocks--;
}

void mminit()
{
	// register page fault exception handler
	svect(14, pfault);

	struct mboot_info *info = (struct mboot_info *) mboot_info;
	meminfo.size = info->mem_upper;
	meminfo.max_blocks = meminfo.size * 1024 / PAGE_SIZE;
	meminfo.used_blocks = meminfo.max_blocks; // by default all blocks are used
	meminfo.mmap = (u8 *) kmalloc(meminfo.max_blocks / 8);
	memset(meminfo.mmap, 0, meminfo.max_blocks / 8);

	// get memory map if valid
	if (info->flags >> 6 & 1)
	{
		kprintf("maestro physical memory map:\n");
		/*
		 *	Each multiboot mmap entry is stored as the following:
		 *	0 - size
		 *	4 - base_addr_low
		 *	8 - base_addr_high
		 *	12 - length_low
		 *	16 - length_high
		 *	20 - type
		 */
		struct multiboot_mmap_entry
		{
			u32 size;
			u32 addr_low;
			u32 addr_high;
			u32 len_low;
			u32 len_high;
			u32 type;
			#define MULTIBOOT_MEMORY_AVAILABLE              1
			#define MULTIBOOT_MEMORY_RESERVED               2
			#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
			#define MULTIBOOT_MEMORY_NVS                    4
			#define MULTIBOOT_MEMORY_BADRAM                 5
		};

		uint len = 0;
		struct multiboot_mmap_entry *entry = (struct multiboot_mmap_entry *) info->mmap_addr;
		while (len < info->mmap_length)
		{
			char typestr[32];
			switch (entry->type)
			{
				case MULTIBOOT_MEMORY_AVAILABLE:        strcpy(typestr, "Available");        break;
				case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE: strcpy(typestr, "ACPI Reclaimable"); break;
				case MULTIBOOT_MEMORY_NVS:              strcpy(typestr, "NVS");              break;
				case MULTIBOOT_MEMORY_BADRAM:           strcpy(typestr, "Defective RAM");    break;
				default:                                strcpy(typestr, "Reserved");
			}
			kprintf("0x%8x-0x%8x:	%s\n", entry->addr_low, entry->addr_low + entry->len_low - 1, typestr);
			entry++;
			len += sizeof(struct multiboot_mmap_entry);
		}
	}

	// kpd = (u32 *) kmalloca(sizeof(struct pagedir));
	// memset(kpd, 0, sizeof(kpd));

	// pagetable = (struct pagetab *) kmalloca(sizeof(struct pagetab));

	// // map the first 4MB of memory
	// for(int i = 0; i < 1024; i++)
	// {
	// 	struct page pg;
	// 	pg.present = 1;
	// 	pg.rw = 1;
	// 	pg.user = 0;
	// 	pg.faddr = i;
	// 	pagetable->pages[i] = pg;
	// };

	// // mark the first page directory entry as being present
	// kpd[0] = (u32) pagetable | 3;

	// // map the rest of the page directories to not present
	// for(int i = 1; i < 1024; i++)
	// 	kpd[i] = 2;

	// cpd = kpd;
	// pdsw(kpd);
}
