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
