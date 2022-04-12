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
	PR_EMPTY,
};

struct proc
{
	enum prstate state;
	uintptr_t stkptr;              // current kernel stack pointer
	u8 kstack[PR_STACKSIZE];       // per process kernel stack
	u8 ustack[PR_STACKSIZE];       // user stack
	int pid;                       // process id
	int mask;                      // interrupt state mask
	struct file *ofile[NOFILE];    // open file table
	u32 wakeup;                    // timestamp to wake up process when sleeping
	char name[32];
};

#define isbadpid(pid) (pid >= 0 && pid < NPROC)

// defined in ctxsw.s
extern void ctxsw(void *, void *);

// defined in sched.c
void sched();

void proc_init();
int create(void (*func)(void), const char *);
void ready(int);

#endif    // PROC_H
