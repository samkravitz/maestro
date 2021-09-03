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
#include <intr.h>
#include <kout.h>
#include <proc.h>
#include <kmalloc.h>

#include "string.h"

extern struct proc *proctab[];
extern struct proc *curr;

void kmain()
{
	kout("Welcome to maestro!\n");
	init();

	// initialize null process entry
	struct proc *nullproc = (struct proc *) kmalloc(sizeof(struct proc));
	nullproc->stkptr = 0;
	memcpy(nullproc->name, "null process", strlen("null process") + 1);
	nullproc->name[strlen("null process")] = '\0'; 
	curr = nullproc;
	proctab[99] = nullproc;

	asm("sti");

	while (1)
		asm("hlt");
}
