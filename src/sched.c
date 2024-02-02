/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: sched.c
 * DATE: August 9, 2021
 * DESCRIPTION: pick the next eligible process to run
 */

#include <intr.h>
#include <kprintf.h>
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
	{
		if (pold->state != PR_RUNNING)
			pnew = &nullproc;
		else
			pnew = curr;
	}

	else
		pnew = (struct proc *) dequeue(readyq);

	if (pnew == pold)
	{
		restore(pold->mask);
		return;
	}

	if (pold != &nullproc && pold->state == PR_RUNNING)
	{
		pold->state = PR_READY;
		insert(readyq, pold);
	}

	curr = pnew;
	curr->state = PR_RUNNING;
	kprintf("scheduling %s\n", pnew->name);
	ctxsw(pold, pnew);
	restore(pold->mask);
}
