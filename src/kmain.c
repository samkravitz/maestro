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

#include "malloc.h"
#include "string.h"

extern struct proc *proctab[];
extern struct proc *curr;

void printA()
{
	while (1)
	{
		koutf("%c", 'a');
		asm("hlt");
	}
}

void printB()
{
	while (1)
	{
		koutf("%c", 'b');	
		asm("hlt");
	}
}

void kmain()
{
	kout("Welcome to maestro!\n");
	init();

	struct proc *prA = create(printA, "print A");
	struct proc *prB = create(printB, "print B");

	proctab[0] = prA;
	proctab[1] = prB;

	// initialize null process entry
	struct proc *nullproc = (struct proc *) malloc(sizeof(struct proc));
	nullproc->stkptr = 0;
	memcpy(nullproc->name, "null process", strlen("null process") + 1);
	nullproc->name[strlen("null process")] = '\0'; 

	curr = nullproc;
	asm("sti");

	while (1)
		asm("hlt");
}