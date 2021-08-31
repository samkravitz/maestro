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
	kprintf("Running a test on chars: \n");
	kprintf("\nafter filling str\n");
	kprintf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		kprintf("%c", str[i]);
	}
	kprintf("\n");
	kprintf("memory block free bit (*str): %d\n", get_block_ptr(str)->free);
	kprintf("memory block debug value (*str): %x\n", get_block_ptr(str)->debug);

	kfree(str);
	kprintf("\nafter freeing str (should be the same as above bc its just marked as free):\n");
	kprintf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		kprintf("%c", str[i]);
	}

	memcpy(str, "Big Chungis", strlen("Big Chungis"));
	kprintf("\nafter reassigning str:\n");
	kprintf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		kprintf("%c", str[i]);
	}
	kprintf("\n");
	kprintf("memory block free bit (*str): %d\n", get_block_ptr(str)->free);
	kprintf("memory block debug value (*str): %x\n", get_block_ptr(str)->debug);

	kprintf("Finding 7! (factorial (but filling the array with each number))\n");
	kprintf("filling the array: ");
	int *fac = kmalloc(8 * sizeof(*fac));
	for(int i=1;i<=7;i++){
		fac[i-1] = i;
		if(i < 7) kprintf("%d, ", i);
		else kprintf("%d", i);
	}
	kprintf("\n");
	kprintf("memory block free bit (*fac): %d\n", get_block_ptr(fac)->free);
	kprintf("memory block debug value (*fac): %x\n", get_block_ptr(fac)->debug);

	kprintf("7! = ");
	for(int i=1;i<=7;i++){
		if(i < 7) kprintf("%d * ", fac[i-1]);
		else kprintf("%d", fac[i-1]);
	}
	kprintf("\n");

	int sol = 1;
	for(int i=1;i<7;i++) {
		sol *= fac[i];
	}
	kprintf("7! = %d\n",sol);
	kfree(fac);
	kprintf("freed the array of numbers :)\n");
	kprintf("memory block free bit (*fac): %d\n", get_block_ptr(fac)->free);
	kprintf("memory block debug value (*fac): %x\n", get_block_ptr(fac)->debug);
	

	asm("sti");

	while (1)
		asm("hlt");
}
