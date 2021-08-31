/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Noah Bennett
 *
 * FILE: kmalloc.test.c
 * DATE: Augh 31, 2021
 * DESCRIPTI N: test kmalloc
 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/kmalloc.h"

#include "string.h"

extern struct proc *proctab[];
extern struct proc *curr;

void main()
{
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
