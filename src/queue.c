/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: queue.c
 * DATE: March 30th, 2022
 * DESCRIPTION: queue data structure
 */
#include <queue.h>
#include <kprintf.h>

bool is_empty(queue q)
{
	return first(q) == QTAIL && last(q) == QHEAD;
}

void enqueue(queue q, int pid)
{
	int prev = q[QTAIL].prev;

	q[pid].next = QTAIL;
	q[pid].prev = prev;
	q[prev].next = pid;
	q[QTAIL].prev = pid;
}

int dequeue(queue q)
{
	int pid = q[QHEAD].next;
	q[QHEAD].next = q[pid].next;
	q[q[QHEAD].next].prev = QHEAD;
	return pid;
}

void insert(queue q, int pid, int key)
{
	int n = first(q);
	while (q[n].key <= key)
		n = q[n].next;
	
	int prev = q[n].prev;
	q[pid].next = n;
	q[pid].prev = prev;
	q[pid].key = key;
	q[prev].next = pid;
	q[n].prev = pid;
}

void clearq(queue q)
{
	for (int i = 0; i < NPROC; i++)
	{
		q[i].pid = -1;
		q[i].next = -1;
		q[i].prev = -1;
		q[i].key = -1;
	}

	q[QHEAD].pid = QHEAD;
	q[QHEAD].prev = QTAIL;
	q[QHEAD].next = QTAIL;
	q[QHEAD].key = -1;

	q[QTAIL].pid = QTAIL;
	q[QTAIL].prev = QHEAD;
	q[QTAIL].next = QHEAD;
	q[QTAIL].key = 0x7fffffff;
}

void printq(queue q)
{
	int pid = first(q);
	while (pid != QTAIL)
	{
		kprintf("%d ", pid);
		pid = q[pid].next;
	}
	kprintf("\n");
}
