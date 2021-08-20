#include <proc.h>

#include <intr.h>
#include <pq.h>
#include <kout.h>

extern struct pq *readylist;
extern struct proc *proctab[];
extern struct proc *prntA;
extern struct proc *curr;
extern struct stackframe *currframe;

struct stackframe **currframeptr;

extern void ctxsw(void *, void *);
//void ctxsw();

void sched()
{
	disable();
	static int n = 0;
	struct proc *prptr = curr;
	struct proc *prnew = proctab[n % 2];
	n++;
	koutf("sched\n");
	//koutf("old: 0x%x   new: 0x%x\n", prptr, prnew);
	curr = prnew;
	currframe = curr->frame;
	currframeptr = &currframe;
	//koutf("boutta schedule\n");
	ctxsw(&prptr->frame, &prnew->frame);
	asm("sti");
	enable();

}
