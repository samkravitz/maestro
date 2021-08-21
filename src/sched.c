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

void sched()
{
	static int n = 0;
	struct proc *pptr = curr;
	struct proc *prnew = proctab[n % 2];
	n++;

	curr = prnew;
	ctxsw(&pptr->stkptr, prnew->stkptr);
}
