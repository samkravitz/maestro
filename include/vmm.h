/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: vmm.h
 * DATE: March 15th, 2022
 * DESCRIPTION: virtual memory manager
 */
#ifndef VMM_H
#define VMM_H

#include <maestro.h>

// page size in bytes
#define PAGE_SIZE              4096

// page table size in bytes
#define PAGE_TABLE_SIZE        4096

// page directory size in bytes
#define PAGE_DIR_SIZE          4096

// number of entries in a page table/directory
#define NUM_TABLE_ENTRIES      1024

// physical address the bootloader placed the kernel page directory
#define KPAGE_DIR_BASE         (u8 *) 0x8000

// physical address the bootloader placed the identity page table
#define IDENT_TABLE_BASE       (u8 *) 0x9000

// physical address the bootloader placed the kernel page table
#define KPAGE_TABLE_BASE       (u8 *) 0xa000

// flag bitmasks for pt_entries
#define PT_PRESENT 1
#define PT_WRITABLE 2
#define PT_USER 4
#define PT_ACCESSED 0x20
#define PT_DIRTY 0x40
#define PT_FRAME 0x7ffff000

// page directory entry
struct pde
{
	u8 present  : 1;     // set if page is present in memory
	u8 rw       : 1;     // set if page is writable, otherwise read only
	u8 user     : 1;     // set if this is a user mode page table
	u8 pwt      : 1;     // set if write-through caching is enabled
	u8 pcd      : 1;     // if set, page table will be cached
	u8 accessed : 1;     // set by the cpu if page has been accessed
	u8 rsvd1    : 1;     // reserved by intel
	u8 size     : 1;     // 0 - 4K pages   1 - 4M pages
	u8 rsvd2    : 4;
	u32 addr    : 20;    // address of page table this entry manages
} __attribute__((packed));

// page table entry
struct pte
{
	u8 present  : 1;     // set if page is present in memory
	u8 rw       : 1;     // set if page is writable, otherwise read only
	u8 user     : 1;     // set if this is a user mode page
	u8 rsvd1    : 2;     // reserved by intel
	u8 accessed : 1;     // set by the cpu if page has been accessed
	u8 dirty    : 1;     // set if page has been written to
	u16 rsvd2   : 5;     // reserved by intel
	u32 addr    : 20;    // physical frame address this entry manages
} __attribute__((packed));

void vmm_init();

uintptr_t vmm_create_address_space();
void vmm_map_page(uintptr_t, uintptr_t, unsigned);
void vmm_map_page_in_pdir(uintptr_t, uintptr_t, uintptr_t, unsigned);


#endif // VMM_H
