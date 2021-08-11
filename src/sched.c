#include <proc.h>

#include <intr.h>
#include <pq.h>
#include <kout.h>

extern struct pq *readylist;
extern struct proc *proctab[];
extern struct proc *prntA;
extern int currpid;

extern void ctxsw(void *, void *);

void sched()
{
	disable();
	static u64 count = 0;
	currpid = count & 1;
	count++;
	struct proc *prptr = currproc();
	//struct proc *newproc = (struct proc *) readylist->data;

	//disable();

	//struct proc *p;
	// if (count & 1)
	// 	p = proctab[1];
	// else
	// 	p = proctab[0];

	struct proc *prnew;

	if (currpid == 0)
		prnew = proctab[1];
	else
		prnew = proctab[0];

	koutf("currpid: %d\n", currpid);
	koutf("prcur: %s, prnew: %s\n", prptr->name, prnew->name);
	ctxsw(&prptr->stkptr, &prnew->stkptr);


	// if (newproc)
	// {
	// 	if (newproc == prntA)
	// 		koutf("its printA\n");
	// 	koutf("%s\n", newproc->name);
	// 	rm(&readylist);
	// 	insert(&readylist, newproc, proccmp);
	// 	//ctxsw();
	// }

	enable();

}
