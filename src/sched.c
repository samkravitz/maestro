#include <proc.h>

extern struct proc *proctab[];
extern struct proc *curr;

extern void ctxsw(void *, void *);

void sched()
{
	static int n = 0;
	struct proc *prptr = curr;
	struct proc *prnew = proctab[n % 2];
	n++;

	curr = prnew;
	ctxsw(&prptr->stkptr, &prnew->stkptr);
}
