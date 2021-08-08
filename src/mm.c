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

#include <klog.h>
#include <kmalloc.h>
#include <maestro/string.h>

u32 *kpd; // kernel page directory
u32 *cpd; // current page directory
struct pagetab *pagetable;

// page fault handler
void pfault() { }

void mminit()
{
	regint(14, pfault);

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

	kprintf("paging is enabled\n");
}
