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
#include <kmalloc.h>
#include <kprintf.h>
#include <proc.h>

#include "stdio.h"
#include "string.h"

extern struct proc *curr;
extern struct proc nullproc;

void kmain()
{
	init();
	printf("Welcome to maestro!\n");
	curr = &nullproc;

	// enable interrupts
	asm("sti");

	// become the null process
	while (1)
		asm("hlt");
}
