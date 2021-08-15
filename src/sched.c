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
	static int nn = 0;
	koutf("nn=%d, currpid=%d\n", nn, nn % 2);
	currpid = nn % 2;
	nn += 1;

	//nn++;
	//nn = nn + 1;
	//koutf ("%d\n", currpid);
	struct proc *prptr = currproc();
	//struct proc *newproc = (struct proc *) readylist->data;

	//disable();

	//struct proc *p;
	// if (count & 1)
	// 	p = proctab[1];
	// else
	// 	p = proctab[0];

	struct proc *prnew = (struct proc *) readylist->data;
	rm(&readylist);
	


	// if (currpid == 0)
	// 	prnew = proctab[1];
	// else if (currpid == 1) {
	// 	prnew = proctab[0];
	// } else {
	// 	prnew = proctab[2];
	// }



	koutf("currpid: %d\n", currpid);
	koutf("prcur: %s, prnew: %s\n", prptr->name, prnew->name);
	//while (1);
	ctxsw(&prptr->stkptr, &prnew->stkptr);
	//ctxsw(&prnew->stkptr, &prptr->stkptr);


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
