/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: queue.h
 * DATE: March 30th, 2022
 * DESCRIPTION: queue data structure
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <maestro.h>
#include <proc.h>

struct qnode
{
	int pid;
	int key;
	int next;
	int prev;
};

typedef struct qnode queue[NPROC + 2];

#define QHEAD NPROC
#define QTAIL NPROC + 1

void enqueue(queue, int);
int dequeue(queue);
void insert(queue, int, int);
int peek(queue);
void clearq(queue);
bool is_empty(queue);

#define first(q) (q[QHEAD].next)
#define last(q)  (q[QTAIL].prev)
#define firstkey(q) (first(q).key)

#endif    // QUEUE_H
