/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: proc.h
 * DATE: August 9, 2021
 * DESCRIPTION: process management
 */
#ifndef PROC_H
#define PROC_H

#include <maestro.h>
#include <state.h>

// max number of processes (for now), 
#define NPROC		100

enum prstate
{
	PR_READY,
	PR_RUNNING,
	PR_WAITING,
};

struct proc
{
	uptr stkptr;
	u32	 stack[1024];
	char name[32];
	int pid;
};

struct proc *create(void (*func)(void), const char *);

// defined in ctxsw.s
extern void ctxsw(void *, void *);

// process ready list
extern struct pq *readylist;

static int proccmp(void *a, void *b) { return -1; }

void sched();

#endif // PROC_H
