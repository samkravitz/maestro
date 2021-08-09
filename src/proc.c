#include <proc.h>

#include <pq.h>

int currpid;
struct proc proctab[NPROC];
struct pq *rdylist = NULL;

int proccmp(void *a, void *b)
{
	struct proc *proc1 = (struct proc *) a;	
	struct proc *proc2 = (struct proc *) b;
}

int curr()
{
	return currpid;
}

struct proc *currproc()
{
	return &proctab[curr()];
}

int spawn(int (*func)(void), char *name, int prio)
{
	return 0;
}