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
#define MMAP_SET(bit)		meminfo.mmap[bit / 8]  =  (1 << (bit % 8))
#define MMAP_CLEAR(bit)		meminfo.mmap[bit / 8] &= ~(1 << (bit % 8))
#define MMAP_TEST(bit)		meminfo.mmap[bit / 8] &   (1 << (bit % 8))

// address of multiboot information struct 
extern uptr mboot_info;

struct meminfo meminfo;

u32 *kpd; // kernel page directory
u32 *cpd; // current page directory
struct pagetab *pagetable;

// page fault handler
void pfault() { }

void mminit()
{
	// register page fault exception handler
	svect(14, pfault);

	struct mboot_info *info = (struct mboot_info *) mboot_info;

	meminfo.size = info->mem_upper;
	meminfo.max_blocks = meminfo.size * 1024 / 4096;
	meminfo.used_blocks = 0;
	meminfo.mmap = (u8 *) malloc(meminfo.max_blocks / 8);
	memset(meminfo.mmap, 0, meminfo.max_blocks / 8);

	kpd = (u32 *) kmalloca(sizeof(struct pagedir));
	memset(kpd, 0, sizeof(kpd));

	pagetable = (struct pagetab *) kmalloca(sizeof(struct pagetab));

	// map the first 4MB of memory
	for(int i = 0; i < 1024; i++)
	{
		struct page pg;
		pg.present = 1;
		pg.rw = 1;
		pg.user = 0;
		pg.faddr = i;
		pagetable->pages[i] = pg;
	};

	// mark the first page directory entry as being present
	kpd[0] = (u32) pagetable | 3;

	// map the rest of the page directories to not present
	for(int i = 1; i < 1024; i++)
		kpd[i] = 2;

	cpd = kpd;
	pdsw(kpd);
}
