/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: pq.h
 * DATE: April 4th, 2022
 * DESCRIPTION: Priority queue implementation
 */
#ifndef PQ_H
#define PQ_H

struct pq
{
	void *data;
	struct pq *next;
};

void push(struct pq **, void *, int (*)(void *, void *));
struct pq *pop(struct pq **);
void *peek(struct pq **);
struct pq *newpq();

#endif    // PQ_H
