/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: sched.c
 * DATE: August 9, 2021
 * DESCRIPTION: pick the next eligible process to run
 */
#include <proc.h>
#include <kout.h>

extern struct proc *proctab[];
extern struct proc *curr;
extern struct proc nullproc;
extern int nproc;

void sched()
{
	struct proc *pold = curr;
	struct proc *pnew;

	int next_pid;
	if (curr->pid == -1)
		next_pid = 0;
	else
		next_pid = (curr->pid + 1) % nproc;
	pnew = proctab[next_pid];

	// null process
	if (nproc == 0)
		pnew = &nullproc;

	if (pnew == pold)
		return;

	curr = pnew;
	ctxsw(&pold->stkptr, pnew->stkptr);
}
