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
#include <queue.h>

// #include "malloc.h"
#include "string.h"

// defined in intr.s
extern u32 isr_end;

struct proc *curr;
struct proc proctab[NPROC];

// process queues
queue readyq, sleepq;

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

// where processes return when they terminate
static void pterm()
{
	kprintf("%s returned!\n", curr->name);
	nproc--;
}

void proc_init()
{
	for (int i = 0; i < NPROC; i++)
	{
		proctab[i].state = PR_EMPTY;
		proctab[i].stkptr = 0;
		proctab[i].pid = -1;
		proctab[i].mask = 0;
		proctab[i].wakeup = 0;
	}

	clearq(readyq);
	clearq(sleepq);
}

/**
 * @brief adds a process to the ready queue
 * @param pid pid of process to ready
 */
void ready(int pid)
{
	proctab[pid].state = PR_READY;
	enqueue(readyq, pid);
}

/**
 * @brief creates a new process in the suspended state 
 * @param f function where the new process will begin execution
 * @param name name of the new process
 * @return pid of new process
 */
int create(void (*f)(void), const char *name)
{
	int pid = next_pid;
	struct proc *pptr = &proctab[pid];
	pptr->pid = pid;
	next_pid++;
	strncpy(pptr->name, name, 32);
	pptr->mask = 0;
	pptr->state = PR_SUSPENDED;
	
	u32 *kstack = (u32 *) (pptr->kstack + PR_STACKSIZE);
	u32 *ustack = (u32 *) (pptr->ustack + PR_STACKSIZE);

	--ustack; *ustack = (u32) pterm;

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
	kstack--; *kstack = (u32) ustack;    // esp
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
	kstack--; *kstack = (u32) &isr_end;  // ctxsw return address

	kstack--; *kstack = 0;               // ebp
	kstack--; *kstack = 0;               // ebx
	kstack--; *kstack = 0;               // esi
	kstack--; *kstack = 0;               // edi

	pptr->stkptr = (uintptr_t) kstack;

	nproc++;
	return pid;
}
