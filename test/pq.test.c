#include "../include/pq.h"
// #include "../include/kmalloc.h"
#include <stdio.h>
#include <stdlib.h>

void insert(struct pq **head, void *data, int (*cmp)(void *a, void *b))
{
	// node to insert
	struct pq *ins = newpq(data);

	// special case - insnode belongs at head of list
	if (cmp((*head)->data, ins->data) > 0)
	{
		ins->next = *head;
		*head     = ins;
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
	*head          = tmp->next;
	return tmp;
}

struct pq *newpq(void *data)
{
	struct pq *pq = (struct pq *) malloc(sizeof(struct pq));
	pq->data      = data;
	pq->next      = NULL;
	return pq;
}

// compare integers
int intcmp(void *a, void *b)
{
	int *ap = (int *) a;
	int *bp = (int *) b;

	if (*ap > *bp)
		return -1;

	if (*ap < *bp)
		return 1;

	return 0;
}

void printpq(struct pq *head)
{
	struct pq *tmp = head;
	while (tmp)
	{
		int *p = (int *) tmp->data;
		printf("%d->", *p);
		tmp = tmp->next;
	}
	printf("NULL\n");
}

int main()
{
	int zero        = 0;
	int one         = 1;
	int two         = 2;
	int three       = 3;
	int four        = 4;
	int five        = 5;
	struct pq *head = newpq(&one);

	// test insert
	printf("Inserting 2...\n");
	insert(&head, &two, intcmp);
	printpq(head);

	printf("\n");
	printf("Inserting 3...\n");
	insert(&head, &three, intcmp);
	printpq(head);

	printf("\n");
	printf("Inserting 5...\n");
	insert(&head, &five, intcmp);
	printpq(head);

	printf("\n");
	printf("Inserting 0...\n");
	insert(&head, &zero, intcmp);
	printpq(head);

	printf("\n");
	printf("Inserting 4...\n");
	insert(&head, &four, intcmp);
	printpq(head);

	// test remove
	printf("\n");
	printf("Removing...\n");
	dequeue(&head);
	printpq(head);

	printf("\n");
	printf("Removing...\n");
	dequeue(&head);
	printpq(head);

	printf("\n");
	printf("Removing...\n");
	dequeue(&head);
	printpq(head);

	printf("\n");
	printf("Removing...\n");
	dequeue(&head);
	printpq(head);

	printf("\n");
	printf("Removing...\n");
	dequeue(&head);
	printpq(head);

	printf("\n");
	printf("Removing...\n");
	dequeue(&head);
	printpq(head);
}