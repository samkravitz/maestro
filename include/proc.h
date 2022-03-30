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
#include <vfs.h>

// max number of processes (for now),
#define NPROC  100

// max number of files a process can open
#define NOFILE 8

#define PR_STACKSIZE 4096

enum prstate
{
	PR_READY,
	PR_RUNNING,
	PR_WAITING,
	PR_SLEEPING,
	PR_SUSPENDED,
};

struct proc
{
	enum prstate state;
	uintptr_t stkptr;              // current stack pointer
	u8 stack[PR_STACKSIZE];
	int pid;                       // process id
	int mask;                      // interrupt state mask
	struct file *ofile[NOFILE];    // open file table
	char name[32];
};

// defined in ctxsw.s
extern void ctxsw(void *, void *);

// defined in sched.c
void sched();

void proc_init();
struct proc *create(void (*func)(void), const char *);
void ready(struct proc *);

#endif    // PROC_H
