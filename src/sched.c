#include <proc.h>

extern struct pq *rdylist;
extern struct proc proctab[];

void sched()
{
	struct proc *prptr = currproc();
}