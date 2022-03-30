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

enum prstate
{
	PR_READY,
	PR_RUNNING,
	PR_WAITING,
};

struct proc
{
	uptr stkptr;
	u32 stack[1024];
	int pid;
	struct file *ofile[NOFILE];    // open file table
	char name[32];
};

struct proc *create(void (*func)(void), const char *);

// defined in ctxsw.s
extern void ctxsw(void *, void *);

// defined in sched.c
void sched();

void proc_init();

#endif    // PROC_H
