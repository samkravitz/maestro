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

extern struct proc *proctab[];
extern struct proc *curr;
extern int nproc;

void sched()
{
	static int n = 0;
	struct proc *pold = curr;
	struct proc *pnew;

	// null process
	if (nproc == 0)
		pnew = proctab[99];

	curr = pnew;
	ctxsw(&pold->stkptr, pnew->stkptr);
}
