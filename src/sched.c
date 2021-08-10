#include <proc.h>

#include <intr.h>
#include <pq.h>
#include <kout.h>

extern struct pq *readylist;
extern struct proc proctab[];

void sched()
{
	struct proc *prptr = currproc();
	struct proc *newproc = (struct proc *) readylist->data;

	disable();

	if (newproc)
	{
		koutf("sched\n");
		rm(&readylist);
		insert(&readylist, newproc, proccmp);
		//ctxsw();
	}

}
