/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kmain.c
 * DATE: July 26, 2021
 * DESCRIPTION: Where it all begins
 */
#include <init.h>
#include <kout.h>
#include <proc.h>
#include <pq.h>

#include "malloc.h"
#include "string.h"

extern struct pq *rdylist;

void kmain()
{
	kout("Welcome to maestro!\n");
	init();

	// set up null process
	struct proc *null = (struct proc *) malloc(sizeof(struct proc));
	u32 stkptr = (u32) malloc(1024);
	null->pid = 0;
	null->prio = -20;
	null->stkbase = stkptr;
	null->stkptr = stkptr;
	null->prstate = PR_READY;
	memcpy(null->name, "null process\0", strlen("null process") + 1);

	// create ready list
	rdylist = newpq(null);

	while (1)
		asm("hlt");
}