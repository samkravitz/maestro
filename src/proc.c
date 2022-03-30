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

struct proc *curr;
struct proc *proctab[NPROC];

// process ready queue
struct queue *readyq;

const struct proc nullproc = {
	.state = PR_RUNNING,
	.stkptr = 0,
	.pid = -1,
	.mask = 0,
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
	readyq = newq();
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
	pptr->state = PR_SUSPENDED;
	
	u32 *stkptr = (u32 *) (pptr->stack + PR_STACKSIZE);

	// create dummy stack frame that ctxsw can return to
	// only need callee-saved registers
	stkptr--; *stkptr = (uintptr_t) pterm;  // return address of process
	stkptr--; *stkptr = (uintptr_t) f;      // eip - where the process will begin execution
	stkptr--; *stkptr = 0x202;              // eflags (start with interrupts enabled)
	stkptr--; *stkptr = 0;                  // ebp
	stkptr--; *stkptr = 0;                  // ebx
	stkptr--; *stkptr = 0;                  // esi
	stkptr--; *stkptr = 0;                  // edi
	pptr->stkptr = (uintptr_t) stkptr;
	pptr->pid = next_pid++;

	nproc++;
	return pptr;
}
