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

#include "string.h"
#include <intr.h>
#include <kmalloc.h>
#include <kout.h>

// macros to manipulate bitmap
#define MMAP_SET(bit)   (meminfo.mmap[bit / 8] = (1 << (bit % 8)))
#define MMAP_CLEAR(bit) (meminfo.mmap[bit / 8] &= ~(1 << (bit % 8)))
#define MMAP_TEST(bit)  (meminfo.mmap[bit / 8] & (1 << (bit % 8)))

struct meminfo meminfo;

u32 *kpd;    // kernel page directory
u32 *cpd;    // current page directory
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
