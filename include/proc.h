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

struct stackframe
{
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 eflags;
	u32 eip;
} __attribute__((packed));

struct proc
{
	struct stackframe *frame;
	char name[32];
	u32	stack[1024];
	uptr stkptr;
} __attribute__((packed));

struct proc *create(void (*func)(void), const char *);

// process ready list
extern struct pq *readylist;

static int proccmp(void *a, void *b) { return -1; }

void sched();

#endif // PROC_H
