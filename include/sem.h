/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: sem.h
 * DATE: April 4th, 2022
 * DESCRIPTION: Counting semaphore implementation
 */

#ifndef SEM_H
#define SEM_H

#include <queue.h>

struct sem
{
	int count;
	queue waitq;
};

void sem_init();
void wait();
void signal();

#endif    // SEM_H
