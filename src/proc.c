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

#include "malloc.h"
#include "string.h"

struct proc *curr;
struct proc *proctab[NPROC];

// number of active processes
int nproc = 0;

// where processes return when they terminate
static void pterm()
{
	koutf("%s returned!\n", curr->name);
	nproc--;
}

struct proc *create(void (*func)(void), const char *name)
{
	struct proc *pptr = (struct proc *) malloc(sizeof(struct proc));
	memcpy(pptr->name, name, strlen(name) + 1);
	pptr->name[strlen(name)] = '\0'; 
	
	u32 *stkptr = pptr->stack + 1024;

	// create dummy stack frame that ctxsw can return to
	// only need callee-saved registers
	stkptr--; *stkptr = pterm;		// return address of process
	stkptr--; *stkptr = func;		// eip - where the process will begin execution
	stkptr--; *stkptr = 0x202;		// eflags (start with interrupts enabled)
	stkptr--; *stkptr = 0;			// ebp
	stkptr--; *stkptr = 0;			// ebx
	stkptr--; *stkptr = 0;			// esi
	stkptr--; *stkptr = 0;			// edi
	pptr->stkptr = stkptr;

	nproc++;
	return pptr;
}