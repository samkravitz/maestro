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

// physical address of kernel start and end
extern u32 start_phys, end_phys;

// size of kernel image in bytes
extern u32 size;

// total size of physical memory in bytes (both available and reserved)
static u32 mem_size = 0;

// number of currently allocated blocks
static u32 used_blocks;

// max number of blocks the system can allocate
// this should always be equivalent to mem_size / BLOCK_SIZE
static u32 max_blocks;

// bitmap of block allocation status
// a set bit denotes the block is used
// the bitmap is placed immediately after the kernel image in memory so we don't have to call kmalloc() to dynamically allocate it
// the kernel heap will be placed right after the bitmap
// it's like a free call to kmalloc :)
static u32 *mmap = &end;

// block the kernel starts/ends on
// i.e. start_block * BLOCK_SIZE = start_phys
// and    end_block * BLOCK_SIZE = end_phys
static u32 start_block, end_block;

extern void *heap;

void pmm_init()
{
	// print out the physical memory map
	kprintf("\tMEMORY MAP\n");
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

	kprintf("\n");

	// the linker should block align &end but it would be trouble if it's not
	if ((uptr) &end % BLOCK_SIZE != 0)
	{
		kprintf("&end was not block-aligned!\n");
	}

	start_block = (u32) &start_phys / BLOCK_SIZE;
	end_block   = (u32) &end_phys   / BLOCK_SIZE;

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
			u32 block = base / BLOCK_SIZE;
			for (size_t i = 0; i < len / BLOCK_SIZE; i++)
				BITMAP_CLEAR(mmap, block + i);
		}

		ent++;
	}

	// after this, the kernel's blocks will be marked as clear, so we have to re-reserve them
	// calculate kernel size in blocks
	u32 kernel_blocks = (u32) &size / BLOCK_SIZE;
	for (size_t i = 0; i < kernel_blocks; i++)
		BITMAP_SET(mmap, start_block + i);
	
	// same with mmap
	// how many blocks the mmap itself takes up
	u32 mmap_blocks = BLOCK_ALIGN(max_blocks) / BLOCK_SIZE;
	for (size_t i = 0; i < mmap_blocks; i++)
		BITMAP_SET(mmap, end_block + i);

	// kernel heap can begin immediately after mmap (on a block-aligned boundary)
	heap = (void *) ((u8 *) mmap + mmap_blocks * BLOCK_SIZE);

	// finally, print out calculated memory stats
	int free_blocks = 0;
	for (size_t block = 0; block < max_blocks; block++)
	{
		if (!BITMAP_TEST(mmap, block))
			free_blocks++;
        else
            used_blocks++;
	}

	kprintf("Installed memory: %d bytes (%d blocks)\n", max_blocks * BLOCK_SIZE, max_blocks);
	kprintf("Free memory: %d bytes (%d blocks)\n", free_blocks * BLOCK_SIZE, free_blocks);
}

/**
 * @brief allocate a block of physical memory
 * @return physical address of allocated block
 */
uintptr_t pmm_alloc()
{
	int idx = BITMAP_FIRST_CLEAR(mmap, max_blocks);

	if (idx < 0)
	{
		kprintf("pmm_alloc: out of physical memory!\n");
		return (uintptr_t) -1;
	}

	BITMAP_SET(mmap, idx);
	return idx * BLOCK_SIZE;
}
