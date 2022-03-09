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

#include <kprintf.h>

#include "string.h"

// physical address where bootloader placed the memory map
#define MMAP_BASE       0x4000

// magic number bootloader put in mmap to signify the end
#define MMAP_MAGIC      0xcafebabe

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

        kprintf("0x%8x-0x%8x: | 0x%8x | %s (%d)\n", base, base + len - 1, len, typestr, ent->type);
        ent++;
    }
}
