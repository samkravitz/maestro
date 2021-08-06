#include <mm.h>

#include <frame.h>
#include <klog.h>
#include <kmalloc.h>
#include <maestro/string.h>

struct pagedir *kpd; // kernel page directory
struct pagedir *cpd; // current page directory

extern u32 heap;

void switch_page_directory(struct pagedir *dir)
{
   cpd = dir;
   kprintf("hi %x %x\n", dir->physaddr, &dir->physaddr);
   asm volatile("mov %0, %%cr3":: "r"(&dir->tables));
   u32 cr0;
   asm volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; // Enable paging!
   kprintf("cr0: %x", cr0);
//    for (int i = 0; i < 10; i++)
//    {
// 	   kprintf("%x\n", dir->phystab[i]);
//    }
   asm volatile("mov %0, %%cr0":: "r"(cr0));
   //asm("mov %0, %%cr0" : : "r" (cr0));
   asm("jmp .");

}

struct page *get_page(u32 address, int make, struct pagedir *dir)
{
	//kprintf("addr: %x\n", address);
   // Turn the address into an index.
   // Find the page table containing this address.
   u32 table_idx = address / 1024;
   if (dir->tables[table_idx]) // If this table is already assigned
   {
       return &dir->tables[table_idx]->pages[address%1024];
   }
   else if(make)
   {
       u32 tmp;
       dir->tables[table_idx] = (struct pagetab*) kmallocap(sizeof(struct pagetab), 1, &tmp);
	   kprintf("poop %x\n", tmp);
       memset(dir->tables[table_idx], 0, 0x1000);
       dir->phystab[table_idx] = tmp | 0x7; // PRESENT, RW, US.
       return &dir->tables[table_idx]->pages[address%1024];
   }
   else
   {
       return 0;
   }
} 

void alloc_frame(struct page *page, int is_kernel, int is_writeable)
{
	if (page->faddr != 0)
	{
		return; // Frame was already allocated, return straight away.
	}
	else
	{
		int idx = fff(); // idx is now the index of the first free frame.
		//kprintf("fff %d\n", idx);
		if (idx == (u32)-1)
		{
			// PANIC is just a macro that prints a message to the screen then hits an infinite loop.
			//PANIC("No free frames!");
			kprintf("PANIC\n");
		}
		set_frame(idx);					   // this frame is now ours!
		page->present = 1;				   // Mark it as present.
		page->rw = (is_writeable) ? 1 : 0; // Should the page be writeable?
		page->user = (is_kernel) ? 0 : 1;  // Should the page be user-mode?
		page->faddr = idx;
	}
}

// Function to deallocate a frame.
void free_frame(struct page *page)
{
	u32 frame;
	if (!(frame = page->faddr))
	{
		return; // The given page didn't actually have an allocated frame!
	}
	else
	{
		clear_frame(frame); // Frame is now free again.
		page->faddr = 0;	// Page now doesn't have a frame.
	}
}

void mminit()
{
	frame_init();

	kpd = (struct pagedir *) kmalloca(sizeof(struct pagedir));
	cpd = kpd;
	memset(kpd, 0, sizeof(struct pagedir));

	u32 i = 0;
	int k = 0;
	while (i < 100)
	{
		//kprintf("i=%x heap=%x\n", i, heap);
		//kprintf("%d\n", k++);
		//Kernel code is readable but not writeable from userspace.
		alloc_frame(get_page(i, 1, kpd), 0, 0);
		i += 1;
	}
	kprintf("Initialized memory2!\n");
	// Before we enable paging, we must register our page fault handler.
	regint(14, pfault);

	kprintf("Initialized memory1!\n");
	//while(1);
	// Now, enable paging!
	//pdsw(kpd, kpd->physaddr);
	switch_page_directory(kpd);

	kprintf("Initialized memory!\n");
	while (1);
}

void pfault()
{
    kprintf("Page fault\n");
}