#include <proc.h>

#include <pq.h>

int currpid;
struct proc *proctab[NPROC];
struct pq *readylist = NULL;

int curr()
{
	return currpid;
}

struct proc *currproc()
{
	return proctab[curr()];
}

int spawn(int (*func)(void), char *name, int prio)
{
	return 0;
}
