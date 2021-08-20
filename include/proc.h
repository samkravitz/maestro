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

// struct proc
// {
// 	char			name[32];
// 	int				pid;
// 	int 			prio;
// 	void		   *stkbase;
// 	void		   *stkptr;
// 	enum prstate 	state;
// 	u32				pc;
// };

struct proc
{
	// u32    esp;
	// u32    ss;
	// u32    kernelEsp;
	// u32    kernelSs;
	// //struct _process*  parent;
	// u32    priority;
	// int         state;
	// char			name[32];
	u8	stack[4096];
	struct stackframe *frame;
	//ktime_t     sleepTimeDelta;
};

// struct stackframe
// {
// 	u32 gs;
// 	u32 fs;
// 	u32 es;
// 	u32 ds;
// 	u32 eax;
// 	u32 ebx;
// 	u32 ecx;
// 	u32 edx;
// 	u32 esi;
// 	u32 edi;
// 	u32 esp;
// 	u32 ebp;
// 	u32 eip;
// 	u32 cs;
// 	u32 flags;
// };

struct stackframe
{
	u32 eax;
	u32 ebx;
	u32 ecx;
	u32 edx;
	u32 esi;
	u32 edi;
	u32 ebp;

	// u32 gs;
	// u32 fs;
	// u32 es;
	// u32 ds;
	//u32 ecode;

	u32 eip;
	u32 cs;
	u32 eflags;
	u32 esp;
	u32 ss;
} __attribute__((packed));

struct proc *create(void (*func)(void));

// process ready list
extern struct pq *readylist;

static int proccmp(void *a, void *b) { return -1; }

void sched();
struct proc *prspawn(void (*)(void), char *,int);
void prret();

#endif // PROC_H
