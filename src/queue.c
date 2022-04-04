/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: queue.c
 * DATE: March 30th, 2022
 * DESCRIPTION: queue data structure
 */
#include <kmalloc.h>
#include <queue.h>

void insert(struct queue *q, void *data)
{
	// node to insert
	struct qnode *node = (struct qnode *) kmalloc(sizeof(struct qnode));
	node->data = data;
	node->next = NULL;

	if (is_empty(q))
	{
		q->front = node;
		q->rear = node;
	}

	else
	{
		q->rear->next = node;
		q->rear = node;
	}

	q->count++;
}

void *dequeue(struct queue *q)
{
	struct qnode *tmp = q->front;
	void *data = tmp->data;
	q->front = q->front->next;
	q->count--;
	kfree(tmp);
	return data;
}

struct queue *newq()
{
	struct queue *queue = (struct queue *) kmalloc(sizeof(struct queue));
	queue->count = 0;
	queue->front = NULL;
	queue->rear  = NULL;
	return queue;
}

bool is_empty(struct queue *q)
{
	return q->count == 0;
}

//void print_queue(struct queue *q)
//{
//	struct qnode *tmp = q->front;
//	while (tmp)
//	{
//		struct proc *ptr = (struct proc *) tmp->data;
//		kprintf("%s->", ptr->name);
//		tmp = tmp->next;
//	}
//	kprintf("\n");
//}
