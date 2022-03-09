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
#include <kprintf.h>

// physical address where bootloader placed the memory map
#define MMAP_BASE       0x4000

// magic number bootloader put in mmap to signify the end
#define MMAP_MAGIC      0xcafebabe

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

    // print out the physical memory map
    kprintf("\n    MEMORY MAP\n");
    kprintf("Region                 | Length     | Type\n");

    /*
    *	Each mmap entry is stored as the following:
    *	 0 - base address
    *	 8 - length of region (in bytes)
    *   16 - region type
    *	20 - attributes (mostly unused except for bit 1 but we're ignoring that for now...)
    */
    struct mmap_entry
    {
        u64 base;
        u64 len;
        u32 type;
        u32 attr;

        #define MMAP_MEMORY_AVAILABLE        1
        #define MMAP_MEMORY_RESERVED         2
        #define MMAP_MEMORY_ACPI_RECLAIMABLE 3
        #define MMAP_MEMORY_NVS              4
        #define MMAP_MEMORY_BADRAM           5
    };

    struct mmap_entry *ent = (struct mmap_entry *) MMAP_BASE;
    char typestr[32];

    while ((u32) ent->base != MMAP_MAGIC)
    {
        // cast to 32 bit to avoid tricky implicit conversions...
        u32 base = (u32) ent->base;
        u32 len  = (u32) ent->len;

        switch (ent->type)
        {
            case MMAP_MEMORY_AVAILABLE:
                strcpy(typestr, "Available");
                break;
            case MMAP_MEMORY_RESERVED:
                strcpy(typestr, "Reserved");
                break;
            case MMAP_MEMORY_ACPI_RECLAIMABLE:
                strcpy(typestr, "ACPI Reclaimable");
                break;
            case MMAP_MEMORY_NVS:
                strcpy(typestr, "NVS");
                break;
            case MMAP_MEMORY_BADRAM:
                strcpy(typestr, "Defective RAM");
                break;
            default:
                strcpy(typestr, "Invalid mmap type");
                break;
        }

        kprintf("0x%8x-0x%8x: | 0x%8x | %s (%d)\n", base, base + len - 1, len, typestr, ent->type);
        ent++;
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
