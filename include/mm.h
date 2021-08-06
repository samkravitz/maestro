#ifndef MM_H
#define MM_H

#include <maestro.h>

struct page
{
	u8 present 	:	1;	// set if page is present in memory
	u8 rw		:	1;	// if set, page is writable
	u8 user		:	1;	// set if this is a user mode page
	u8 rsvd1	:	2;
	u8 accessed	:	1;	// set by the cpu if page has been accessed
	u8 dirty	:	1;	// set if page has been written to
	u8 rsvd2	:	2;
	u8 avail	:	3;	// these 3 bits are unused
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
	struct pagetab *pages[1024];
	u32 phystab[1024];
	u32 physaddr;
};

// defined in pdsw.s
extern void pdsw(struct pagedir *pd, u32 phys);

void mminit();

#endif // MM_H