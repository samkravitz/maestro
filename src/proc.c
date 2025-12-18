/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: proc.h
 * DATE: August 9, 2021
 * DESCRIPTION: process management
 */
#include <proc.h>
#include <intr.h>
#include <kmalloc.h>
#include <kprintf.h>
#include <pmm.h>
#include <pq.h>
#include <queue.h>
#include <vmm.h>

#include <string.h>

// defined in intr.s
extern u32 isr_end;

// defined in elf.c
extern void run_elf();

struct proc *curr;
struct proc *proctab[NPROC];

// process ready queue
struct queue *readyq;

// process sleep queue
struct pq *sleepq;

struct proc nullproc = {
	.state = PR_RUNNING,
	.stkptr = 0,
	.pid = -1,
	.mask = 0,
	.wakeup = 0,
	.name = "null process",
};

// number of active processes
int nproc = 0;

int next_pid = 0;

void proc_init()
{
	readyq = newq();
	sleepq = newpq();
}

/**
 * @brief adds a process to the ready queue
 * @param pptr process pointer to ready
 */
void ready(struct proc *pptr)
{
	pptr->state = PR_READY;
	insert(readyq, pptr);
}

struct proc *create_usermode(const char *path)
{
    return create(run_elf, path);
}

/**
 * @brief creates a new process in the suspended state 
 * @param f function where the new process will begin execution
 * @param name name of the new process
 */
struct proc *create(void (*f)(void), const char *name)
{
	struct proc *pptr = (struct proc *) kmalloc(sizeof(struct proc));
	strncpy(pptr->name, name, 32);
	pptr->mask = 0;
	pptr->pdir = 0;  // 0 means use current page directory (kernel pdir)
	pptr->state = PR_SUSPENDED;
	pptr->parent = 0;
	pptr->waiting_for = -1;
	
	u32 *kstack = (u32 *) (pptr->kstack + PR_STACKSIZE);
	pptr->stkbtm = (uintptr_t) kstack;


	/**
	 * create a dummy kernel stack frame that ctxsw can return from
	 * the idea is to set up the frame such that it looks like it is returning from a clock irq
	 * the stack frame looks like this (higher addresses first):
	 * 
	 * [pushed by x86]
	 * ss
	 * esp [0]
	 * eflags
	 * cs
	 * eip [1]
	 * 
	 * [pushed by clock irq]
	 * errcode
	 * intr_num
	 * 
	 * [pushed by isr_bootstrap]
	 * ds
	 * es
	 * fs
	 * gs
	 * general purpose regs
	 * struct registers *
	 * 
	 * [pushed by ctxsw]
	 * ebp
	 * ebx
	 * esi
	 * edi
	 * 
	 * [0] - esp at time of interrupt (that is, the user esp)
	 * [1] - eip for interrupt to return to (that is, the process's main())
	 */
	kstack--; *kstack = 0x23;            // ss
	kstack--; *kstack = 0;               // esp
	kstack--; *kstack = 0x202;           // eflags
	kstack--; *kstack = 0x1b;            // cs
	kstack--; *kstack = (u32) f;         // eip
	kstack--; *kstack = 0;               // errcode
	kstack--; *kstack = 0;               // intrnum
	kstack--; *kstack = 0x20;            // ds
	kstack--; *kstack = 0x20;            // es
	kstack--; *kstack = 0x20;            // fs
	kstack--; *kstack = 0x20;            // gs

	// pushed by push instruction
	kstack--; *kstack = 0;
	kstack--; *kstack = 0;
	kstack--; *kstack = 0;
	kstack--; *kstack = 0;
	kstack--; *kstack = 0;
	kstack--; *kstack = 0;
	kstack--; *kstack = 0;
	kstack--; *kstack = 0;

	kstack--;                            // struct registers *
	kstack--; *kstack = (u32) f;         // ctxsw return address

	kstack--; *kstack = 0;               // ebp
	kstack--; *kstack = 0;               // ebx
	kstack--; *kstack = 0;               // esi
	kstack--; *kstack = 0;               // edi

	pptr->stkptr = (uintptr_t) kstack;
	pptr->pid = next_pid++;
	proctab[pptr->pid] = pptr;

	nproc++;
	return pptr;
}

void proc_exit(int status)
{
    kprintf("%s (pid = %d) exited with code %d\n", curr->name, curr->pid, status);
    // TODO - unmap and free all memory used by the process
    curr->state = PR_TERMINATED;
    nproc--;

	// check if parent process is waiting for this one
	if (curr->parent != -1)
	{
		struct proc *parent = proctab[curr->parent];
		if (parent->state == PR_WAITING && parent->waiting_for == curr->pid)
		{
			parent->waiting_for = -1;
			ready(parent);
		}
	}

    sched();
}

/**
 * @brief fork the current process
 * @param regs saved registers from syscall
 * @return child pid to parent, 0 to child, -1 on error
 */
int proc_fork(struct registers *regs)
{
	// Access iret frame fields beyond struct registers
	u32 *stack = (u32 *) regs;
	u32 user_cs = stack[15];
	u32 user_eflags = stack[16];
	u32 user_esp = stack[17];
	u32 user_ss = stack[18];

	// Allocate new process struct
	struct proc *child = (struct proc *) kmalloc(sizeof(struct proc));
	if (!child)
		return -1;

	// Copy process metadata

	strncpy(child->name, curr->name, 32);
	child->mask = curr->mask;
	child->state = PR_READY;
	child->pid = next_pid++;
	child->sbrk = curr->sbrk;
	child->ustack = curr->ustack;
	child->wakeup = 0;
	child->parent = curr->pid;
	child->waiting_for = -1;

	// Copy open files
	for (int i = 0; i < NOFILE; i++)
		child->ofile[i] = curr->ofile[i];

	// Create new address space
	uintptr_t child_pdir = vmm_create_address_space();
	if (!child_pdir)
	{
		kfree(child);
		return -1;
	}
	child->pdir = child_pdir;

	// Copy user space pages from parent to child
	// Use PDE 769 for temporary mapping (outside user space 0-767, avoids conflict)
	u32 *parent_pdir = (u32 *) 0xfffff000;
	void *page_tables = (void *) 0xffc00000;
	void *temp_map = (void *)(769 << 22);  // Virtual address for PDE 769, PTE 0

	// Create a page table for PDE 769 if it doesn't exist
	u32 saved_pde = parent_pdir[769];
	uintptr_t temp_pt_phys = 0;

	if (!(saved_pde & PT_PRESENT))
	{
		temp_pt_phys = pmm_alloc();
		if (!temp_pt_phys)
		{
			kfree(child);
			return -1;
		}
		parent_pdir[769] = temp_pt_phys | PT_PRESENT | PT_WRITABLE;
		asm volatile("mov %%cr3, %%eax; mov %%eax, %%cr3" ::: "eax", "memory");

		// Zero out the page table
		u32 *temp_pt = (u32 *)(page_tables + 769 * PAGE_SIZE);
		for (int i = 0; i < 1024; i++)
			temp_pt[i] = 0;
	}

	u32 *temp_pt = (u32 *)(page_tables + 769 * PAGE_SIZE);

	for (int pdi = 0; pdi < 768; pdi++)  // User space PDEs (0-767)
	{
		if (!(parent_pdir[pdi] & PT_PRESENT))
			continue;

		u32 *parent_pt = (u32 *)(page_tables + pdi * PAGE_SIZE);

		for (int pti = 0; pti < 1024; pti++)
		{
			if (!(parent_pt[pti] & PT_PRESENT))
				continue;

			uintptr_t virt = ((u32)pdi << 22) | ((u32)pti << 12);
			unsigned flags = parent_pt[pti] & 0xfff;

			// Allocate new physical page for child
			uintptr_t child_phys = pmm_alloc();
			if (!child_phys)
			{
				// TODO: cleanup on failure
				return -1;
			}

			// Temporarily map child's physical page at PTE 0 in our temp page table
			temp_pt[0] = child_phys | PT_PRESENT | PT_WRITABLE;
			asm volatile("invlpg (%0)" :: "r"(temp_map) : "memory");

			// Copy page contents from parent's virtual address to child's page
			memcpy(temp_map, (void *) virt, PAGE_SIZE);

			// Map the new physical page in child's address space
			vmm_map_page_in_pdir(child_pdir, child_phys, virt, flags);
		}
	}

	// Restore PDE 769
	parent_pdir[769] = saved_pde;
	asm volatile("invlpg (%0)" :: "r"(temp_map) : "memory");

	// Set up child's kernel stack
	u32 *kstack = (u32 *) (child->kstack + PR_STACKSIZE);
	child->stkbtm = (uintptr_t) kstack;

	// Build stack frame for child to return from syscall with eax=0
	// Stack is built top-down (high to low address)
	// isr_end expects: segments at lower addr, then general regs, then iret frame

	// iret frame (at highest addresses, popped last by iret)
	kstack--; *kstack = user_ss;
	kstack--; *kstack = user_esp;
	kstack--; *kstack = user_eflags;
	kstack--; *kstack = user_cs;
	kstack--; *kstack = regs->eip;

	// Interrupt frame (skipped by add esp, 8)
	kstack--; *kstack = 0;          // error_code
	kstack--; *kstack = SYSCALL;    // intr_num

	// General purpose registers (popped by popa: edi, esi, ebp, skip, ebx, edx, ecx, eax)
	// Must be at higher addresses than segment registers
	kstack--; *kstack = 0;          // eax = 0 for child!
	kstack--; *kstack = regs->ecx;
	kstack--; *kstack = regs->edx;
	kstack--; *kstack = regs->ebx;
	kstack--; *kstack = regs->esp;  // ignored by popa
	kstack--; *kstack = regs->ebp;
	kstack--; *kstack = regs->esi;
	kstack--; *kstack = regs->edi;

	// Segment registers (popped first by pop gs, fs, es, ds)
	// Must be at lower addresses (closer to where ESP will be)
	kstack--; *kstack = regs->ds;
	kstack--; *kstack = regs->es;
	kstack--; *kstack = regs->fs;
	kstack--; *kstack = regs->gs;

	// struct registers pointer (skipped by add esp, 4)
	kstack--;

	// ctxsw return address - return to isr_end to complete interrupt return
	kstack--; *kstack = (u32) &isr_end;

	// ctxsw callee-saved registers
	kstack--; *kstack = 0;  // ebp
	kstack--; *kstack = 0;  // ebx
	kstack--; *kstack = 0;  // esi
	kstack--; *kstack = 0;  // edi

	child->stkptr = (uintptr_t) kstack;

	// Add child to ready queue
	nproc++;
	ready(child);

	// Return child's PID to parent
	proctab[child->pid] = child;
	return child->pid;
}
