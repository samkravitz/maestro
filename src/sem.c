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
	clearq(sem.waitq);
}

void wait()
{
	int mask = disable();
	if (--sem.count < 0)
	{
		curr->state = PR_WAITING;
		enqueue(sem.waitq, curr->pid);
		sched();
	}
	restore(mask);
}

void signal()
{
	int mask = disable();
	if (++sem.count >= 0)
	{
		if (!is_empty(sem.waitq))
		{
			int pid = dequeue(sem.waitq);
			ready(pid);
			sched();
			restore(mask);
			return;
		}
	}
	restore(mask);
}
