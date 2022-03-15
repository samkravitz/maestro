/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: pmm.c
 * DATE: March 9th, 2022
 * DESCRIPTION: physical memory manager
 */
#include <pmm.h>

#include <bitmap.h>
#include <kprintf.h>

#include "string.h"

// symbols placed at kernel image start and end, respectively
extern u32 start, end;

// total size of physical memory in bytes (both available and reserved)
static u32 mem_size = 0;

// number of currently allocated blocks
static u32 used_blocks;

// max number of blocks the system can allocate
// this should always be equivalent to mem_size / BLOCK_SIZE
static u32 max_blocks;

// bitmap of block allocation status
// a set bit denotes the block is used
// place the bitmap immediately after the kernel image in memory so we don't have to call kmalloc() to dynamically allocate it
// and after we figure out the size of the bitmap the kernel heap is placed right after it
// it's like a free call to kmalloc :)
static u32 *mmap = &end;

// physical address of base of kernel heap
uptr heap;

void pmminit()
{
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

		mem_size += ent->len;

		kprintf("0x%8x-0x%8x: | 0x%8x | %s (%d)\n", base, base + len - 1, len, typestr, ent->type);
		ent++;
	}

	// the linker should block align &end but it would be trouble if it's not
	if ((uptr) &end % BLOCK_SIZE != 0)
	{
		kprintf("&end was not block-aligned!\n");
	}

	// the number of bytes mmap bitmap takes up is conveniently exactly equal to max_blocks
	max_blocks = mem_size / BLOCK_SIZE;

	// mark every block as reserved
	memset(mmap, 0xff, max_blocks);

	// loop back through the memory map and mark available blocks as available
	ent = (struct mmap_entry *) MMAP_BASE;
	while ((u32) ent->base != MMAP_MAGIC)
	{
		// cast to 32 bit to avoid tricky implicit conversions...
		u32 base = (u32) ent->base;
		u32 len  = (u32) ent->len;

		if (ent->type == MMAP_MEMORY_AVAILABLE)
		{
			for (size_t i = 0; i < len / BLOCK_SIZE; ++i)
				BITMAP_CLEAR(mmap, PHYS_TO_MMAP_IDX(base + BLOCK_SIZE * i));
		}

		ent++;
	}

	// after this, the kernel's blocks will be marked as clear, so we have to re-reserve them
	// calculate kernel size in blocks
	u32 kernel_blocks = ((uptr) &end - (uptr) &start) / BLOCK_SIZE;
	for (size_t i = 0; i < kernel_blocks; ++i)
		BITMAP_SET(mmap, PHYS_TO_MMAP_IDX((uptr) &start + BLOCK_SIZE * i));
	
	// same with mmap
	// mmap size in blocks
	u32 mmap_blocks = BLOCK_ALIGN(max_blocks) / BLOCK_SIZE;
	for (size_t i = 0; i < mmap_blocks; ++i)
		BITMAP_SET(mmap, PHYS_TO_MMAP_IDX((uptr) &end + BLOCK_SIZE * i));
	
	// last, we are reserving 1 block for the kernel heap initially, so mark that as reserved too
	BITMAP_SET(mmap, PHYS_TO_MMAP_IDX((uptr) &end + BLOCK_ALIGN(max_blocks)));

	// kernel heap can begin immediately after (on a block-aligned boundary)
	heap = (uptr) &end + BLOCK_ALIGN(max_blocks) + BLOCK_SIZE;

	// finally, print out calculated memory stats
	int free_blocks = 0;
	for (size_t block = 0; block < max_blocks; block++)
	{
		if (!BITMAP_TEST(mmap, block))
			free_blocks++;
        else
            used_blocks++;
	}

	kprintf("\nInstalled memory: %d bytes (%d blocks)\n", max_blocks * BLOCK_SIZE, max_blocks);
	kprintf("Free memory: %d bytes (%d blocks)\n", free_blocks * BLOCK_SIZE, free_blocks);
}
