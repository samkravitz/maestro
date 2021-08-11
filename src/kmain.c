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

extern struct pq *readylist;
extern struct proc *proctab[];

struct proc *prntA;

extern void hijack();

void printA()
{
	while (1)
	{
		koutf("%c", 'A');
		asm("hlt");
	}
}

void printB()
{
	while (1)
	{
		koutf("%c", 'B');	
		asm("hlt");
	}
}

void kmain()
{
	kout("Welcome to maestro!\n");
	init();

	struct proc *prA = prspawn(printA, "print A");
	struct proc *prB = prspawn(printB, "print B");

	// create ready list
	//readylist = newpq(null);

	//insert(&readylist, prntA, proccmp);
	//insert(&readylist, prntB, proccmp);

	proctab[0] = prA;
	proctab[1] = prB;
	//insert(&readylist, printB, proccmp);
	//printA();

	while (1)
		asm("hlt");
}
