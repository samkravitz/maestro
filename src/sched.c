/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: sched.c
 * DATE: August 9, 2021
 * DESCRIPTION: pick the next eligible process to run
 */
#include <kprintf.h>
#include <intr.h>
#include <proc.h>
#include <queue.h>

extern struct proc *proctab[];
extern struct proc *curr;
extern struct proc nullproc;
extern int nproc;
extern struct queue *readyq;

void sched()
{
	struct proc *pold = curr;
	struct proc *pnew;

	// save current interrupt state into current process's mask
	pold->mask = disable();

	if (is_empty(readyq))
		pnew = &nullproc;

	else
	{
		pnew = (struct proc *) dequeue(readyq);
		insert(readyq, pnew);
	}

	if (pnew == pold)
	{
		restore(pold->mask);
		return;
	}

	curr = pnew;
	ctxsw(&pold->stkptr, &pnew->stkptr);
	restore(pold->mask);
}
