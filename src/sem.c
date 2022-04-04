/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: sem.c
 * DATE: April 4th, 2022
 * DESCRIPTION: Counting semaphore implementation
 */

#include <sem.h>

#include <intr.h>
#include <kprintf.h>
#include <proc.h>

extern struct proc *curr;

struct sem sem;

void sem_init()
{
	sem.count = 0;
	sem.waitq = newq();
}

void wait()
{
	int mask = disable();
	if (--sem.count < 0)
	{
		curr->state = PR_WAITING;
		insert(sem.waitq, curr);
		sched();
	}
	restore(mask);
}

void signal()
{
	int mask = disable();
	if (++sem.count >= 0)
	{
		struct proc *pptr = (struct proc *) dequeue(sem.waitq);
		if (!pptr)
		{
			kprintf("signal() - pptr is NULL");
			while (1)
				;
		}
		ready(pptr);
		sched();
	}
	restore(mask);
}
