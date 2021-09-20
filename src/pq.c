/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: pq.c
 * DATE: August 9th, 2021
 * DESCRIPTION: Priority queue implementation
 */
#include <pq.h>
#include <kmalloc.h>

void insert(struct pq **head, void *data, int (*cmp)(void *a, void *b))
{
	// node to insert 
	struct pq *ins = newpq(data);

	// special case - ins belongs at head of list
	if (cmp((*head)->data, ins->data) > 0)
	{
		ins->next = *head;
		*head = ins;
		return;
	}

	struct pq *tmp = *head;
	while (tmp->next && (cmp(tmp->next->data, data) <= 0))
		tmp = tmp->next;

	ins->next = tmp->next;
	tmp->next = ins;
}

struct pq *dequeue(struct pq **head)
{
	struct pq *tmp = *head;
	*head = tmp->next;
	return tmp;
}

struct pq *newpq(void *data)
{
	struct pq *pq = (struct pq *) kmalloc(sizeof(struct pq));
	pq->data = data;
	pq->next = NULL;
	return pq;
}
