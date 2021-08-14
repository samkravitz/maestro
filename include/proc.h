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
	char			name[32];
	int				pid;
	int 			prio;
	void		   *stkbase;
	void		   *stkptr;
	enum prstate 	state;
	u32				pc;
};

// process ready list
extern struct pq *readylist;

static int proccmp(void *a, void *b) { return -1; }

int curr();
struct proc *currproc();
void sched();
struct proc *prspawn(void (*)(void), char *,int);
void prret();

#endif // PROC_H
