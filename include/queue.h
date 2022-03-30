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

struct qnode
{
	void *data;
	struct qnode *next;
};
struct queue
{
	uint count;
    struct qnode *front;
    struct qnode *rear;
};

void insert(struct queue *, void *);
void *dequeue(struct queue *);
struct queue *newq();
bool is_empty(struct queue *);

#endif // QUEUE_H
