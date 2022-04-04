
/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: pq.c
 * DATE: April 4th, 2022
 * DESCRIPTION: Priority queue implementation
 */

#include <pq.h>

#include <kmalloc.h>

static inline bool is_empty(struct pq **head)
{
	return (*head) == NULL;
}

void push(struct pq **head, void *data, int (*cmp)(void *a, void *b))
{
	// node to insert
	struct pq *ins = newpq();
	ins->data = data;

	if (is_empty(head))
	{
		*head = ins;
		return;
	}

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

struct pq *pop(struct pq **head)
{
	struct pq *tmp = *head;
	*head = tmp->next;
	return tmp;
}

void *peek(struct pq **head)
{
	if (is_empty(head))
		return NULL;

	return (*head)->data;
}

struct pq *newpq()
{
	struct pq *pq = (struct pq *) kmalloc(sizeof(struct pq));
	pq->next = NULL;
	pq->data = NULL;
	return pq;
}
