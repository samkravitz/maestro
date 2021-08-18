/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: pq.h
 * DATE: August 9th, 2021
 * DESCRIPTION: Priority queue implementation
 */
#ifndef PQ_H
#define PQ_H

struct pq
{
	void *data;
	struct pq *next;
};

void insert(struct pq **, void *, int (*)(void *, void*));
struct pq *dequeue(struct pq **);
struct pq *newpq(void *);

#endif // PQ_H