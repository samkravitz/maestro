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

#include "malloc.h"
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

	char *str;
	str = kmalloc(11 * sizeof(*str));
	memcpy(str, "Hello world..?", strlen("Hello world..?"));
	kprintf("\nafter filling str\n");
	kprintf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		kprintf("%c", str[i]);
	}

	kfree(str);
	kprintf("\nafter freeing str (should be the same as above bc its just marked as free):\n");
	kprintf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		kprintf("%c", str[i]);
	}

	memcpy(str, "Big Chungis", strlen("Big Chungis"));
	kprintf("after reassigning str:\n");
	kprintf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		kprintf("%c", str[i]);
	}
	kprintf("\n");


	asm("sti");

	while (1)
		asm("hlt");
}