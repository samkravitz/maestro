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

struct registers;

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
	PR_TERMINATED,
};

struct proc
{
	uintptr_t stkptr;              // current kernel stack pointer
	uintptr_t stkbtm;              // address of bottom of kernel stack
	uintptr_t pdir;                // physical address of page directory
	enum prstate state;
	u8 kstack[PR_STACKSIZE];       // per process kernel stack
	void *ustack;                  // user stack
	int pid;                       // process id
	int parent;                    // parent process id
	int waiting_for;               // pid this process is waiting for
	int mask;                      // interrupt state mask
	struct file *ofile[NOFILE];    // open file table
	u32 wakeup;                    // timestamp to wake up process when sleeping
	void *sbrk;                    // address of system break
	char name[32];
};

// defined in ctxsw.s
extern void ctxsw(void *, void *);

// defined in sched.c
void sched();

void proc_init();
struct proc *create(void (*func)(void), const char *);
struct proc *create_usermode(const char *);
void ready(struct proc *);
void proc_exit(int);
int proc_fork(struct registers *);

#endif    // PROC_H
