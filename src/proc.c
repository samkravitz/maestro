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
#include <kmalloc.h>
#include <kprintf.h>
#include <pq.h>
#include <queue.h>

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

const struct proc nullproc = {
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
	memset(pptr->kstack, 0, sizeof(pptr->kstack));
	strncpy(pptr->name, name, 32);
	pptr->mask = 0;
	pptr->state = PR_SUSPENDED;
	
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

	nproc++;
	return pptr;
}

void proc_exit(int status)
{
    kprintf("%s (pid = %d) exited with code %d\n", curr->name, curr->pid, status);
    // TODO - unmap and free all memory used by the process
    curr->state = PR_TERMINATED;
    nproc--;

    sched();
}

static void print() {
	while (1)
		kprintf("hi\n");
}

int proc_fork(struct registers *regs)
{
	struct proc *child = (struct proc *) kmalloc(sizeof(struct proc));
	child->ustack = kmalloc(PR_STACKSIZE);
	memcpy(child->ustack, curr->ustack, PR_STACKSIZE);
	memcpy(child->kstack, curr->kstack, PR_STACKSIZE);
	child->pdir = vmm_clone_directory();
	child->sbrk = curr->sbrk;
	strcpy(child->name, "dup");
	child->pid = next_pid++;

	u32 *kstack = (u32 *) (child->kstack + PR_STACKSIZE);
	child->stkbtm = (uintptr_t) kstack;

	kprintf("%x %x %x %x\n", regs->eip, regs->esp, &isr_end, isr_end);

	kstack--; *kstack = regs->ss;            // ss
	kstack--; *kstack = regs->esp;               // esp
	kstack--; *kstack = regs->eflags;           // eflags
	kstack--; *kstack = regs->cs;            // cs
	kstack--; *kstack = (u32) regs->eip;         // eip
	kstack--; *kstack = 0;               // errcode
	kstack--; *kstack = 0;               // intrnum
	

	// pushed by push instruction
	kstack--; *kstack = regs->edi;
	kstack--; *kstack = regs->esi;
	kstack--; *kstack = regs->ebp;
	kstack--; *kstack = regs->oesp;
	kstack--; *kstack = regs->ebx;
	kstack--; *kstack = regs->edx;
	kstack--; *kstack = regs->ecx;
	kstack--; *kstack = regs->eax;

	kstack--; *kstack = regs->ds;            // ds
	kstack--; *kstack = regs->es;            // es
	kstack--; *kstack = regs->fs;            // fs
	kstack--; *kstack = regs->gs;            // gs

	kstack--;                            // struct registers *
	kstack--; *kstack = (u32) &isr_end;         // ctxsw return address

	kstack--; *kstack = regs->ebp;               // ebp
	kstack--; *kstack = regs->ebx;               // ebx
	kstack--; *kstack = regs->esi;               // esi
	kstack--; *kstack = regs->edi;               // edi

	child->stkptr = (uintptr_t) kstack;

	ready(child);

	//u32 *kstack = (u32 *) curr->ustack;

	//for (int i = 1023; i >= 0; i--)
	//{
	//	kprintf("[%d]: 0x%x\n", i, kstack[i]);
	//}

	//while (1)
	//	;

	//struct proc *child = create(print, curr->name);
	//ready(child);

	return child->pid;
}
