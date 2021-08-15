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

#include <stdint.h>

#include "malloc.h"
#include "string.h"

extern struct pq *readylist;
extern struct proc *proctab[];

struct proc *prntA;

u32 *prAstkptr = (u32*) 0xA000;
u32 *prBstkptr = (u32 *) (0xA000 - 0x400);
u32 *nullstkptr = (u32 *) (0xA000 - 0x400 - 0x400);

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
	// *a= 32;
	// uintptr_t ptr = (uintptr_t) a;
	// koutf("%x\n", (u32) ptr);
	//int c = 45;
	//uintptr_t pc = (uintptr_t) &c;
	//koutf("%x\n", (u32 *) pc);
	//kout ("initialized\n");
	//while (1);
	//*prBstkptr = 4500;
	init();

	struct proc *prA = prspawn(printA, "print A", 1);
	struct proc *prB = prspawn(printB, "print B", 0);
	//struct proc *nullproc = prspawn(null, "null process", 2);

	// create ready list
	//readylist = newpq(null);

	//insert(&readylist, prntA, proccmp);
	//insert(&readylist, prntB, proccmp);

	proctab[0] = prA;
	proctab[1] = prB;
	//proctab[2] = nullproc;
		// enable interrupts
	enable();
	null();

	//insert(&readylist, printB, proccmp);
	//printA();
	//null();

	// while (1) {
	// 	koutf("null proc\n");
	// 	asm("hlt");
	// }
}

void null(){
	while (1) {
		koutf("n");
		asm("hlt");
	}
}
