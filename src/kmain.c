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
#include <pq.h>
#include <dump.h>

#include <stdint.h>

#include "malloc.h"
#include "string.h"

extern struct pq *readylist;
extern struct proc *proctab[];

extern struct proc *curr;
extern struct stackframe *currframe;
extern struct stackframe **currframeptr;


// u32 *prAstkptr = (u32*) 0xA000;
// u32 *prBstkptr = (u32 *) (0xA000 - 0x400);
// u32 *nullstkptr = (u32 *) (0xA000 - 0x400 - 0x400);

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

void null();
void kmain()
{
	kout("Welcome to maestro!\n");
	//koutf("printA: %x\n", printA);
	init();

	struct proc *prA = create(printA);
	struct proc *prB = create(printB);

	proctab[0] = prA;
	proctab[1] = prB;

	u32 arr[] = {1, 2, 3, 4, 5};

	curr = prA;
	currframe = prA->frame;
	currframeptr = &prA->frame;
	// disable();
	// stackdump((u32 *) currframe, 12);
	// while (1);
	//proctab[2] = nullproc;
		// enable interrupts
	asm("sti");
	//sched();
	//null();

	//insert(&readylist, printB, proccmp);
	//printA();
	//null();

	while (1) {
		koutf("null proc\n");
		asm("hlt");
	}
}

void null(){
	while (1) {
		koutf("n");
		asm("hlt");
	}
}
