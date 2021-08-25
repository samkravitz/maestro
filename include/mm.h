/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: mm.h
 * DATE: August 8th, 2021
 * DESCRIPTION: memory management for maestro
 */
#ifndef MM_H
#define MM_H

#include <maestro.h>

#define PAGE_SIZE				4096

struct mboot_info
{
	u32 flags;					// required
	u32 mem_lower;				// present if flags[0] is set
	u32 mem_upper;				// present if flags[0] is set
	u32 boot_device;			// present if flags[1] is set
	u32 cmdline;				// present if flags[2] is set
	u32 mods_count;				// present if flags[3] is set
	u32 mods_addr;				// present if flags[3] is set
	u32 syms[4];				// present if flags[4] or flags[5] is set
	u32 mmap_length;			// present if flags[6] is set
	u32 mmap_addr;				// present if flags[6] is set
	u32 drives_length;			// present if flags[7] is set
	u32 drives_addr;			// present if flags[7] is set
	u32 config_table;			// present if flags[8] is set
	u32 boot_loader_name;		// present if flags[9] is set
	u32 apm_table;				// present if flags[10] is set
	u32 vbe_control_info;		// present if flags[11] is set
	u32 vbe_mode_info;
	u32 vbe_mode;
	u32 vbe_interface_seg;
	u32 vbe_interface_off;
	u32 vbe_interface_len;
	u32 framebuffer_addr;		// present if flags[12] is set
	u32 framebuffer_pitch;
	u32 framebuffer_width;
	u32 framebuffer_height;
	u32 framebuffer_bpp;
	u8 framebuffer_type;
	u8 color_info[5];
};

struct meminfo
{
	uint size;			// size of physical memory in KB
	uint used_blocks;	// number of blocks in use
	uint max_blocks;	// maximum number of allocable blocks
	u8 *mmap;			// bitmap of block status
};

struct page
{
	u8 present 	:	1;	// set if page is present in memory
	u8 rw		:	1;	// if set, page is writable
	u8 user		:	1;	// set if this is a user mode page
	u8 accessed	:	1;	// set by the cpu if page has been accessed
	u8 dirty	:	1;	// set if page has been written to
	u8 rsvd		:	7;	// reserved by intel
	u32 faddr   :	20;	// frame address in physical memory
} __attribute__((packed));

// page table
struct pagetab
{
	struct page pages[1024];
};

// page directory
struct pagedir
{
	struct page *tables[1024];
	//u32 phystab[1024];
	//u32 physaddr;
};

// gets index of first available frame
int first_free_frame();

// allocate a free block
void *balloc();

// deallocate a block
void bfree(void *);

void mminit();
void pfault();

// defined in pdsw.s
extern void pdsw(u32 *pd);

#endif // MM_H