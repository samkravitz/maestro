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

#include <elf.h>

#include <stdio.h>
#include <string.h>

extern struct proc *curr;
extern struct proc nullproc;
extern void clear();

void kmain()
{
	init();
	clear();
	kprintf("Welcome to maestro!\n");
	curr = &nullproc;

    struct proc *init = create_usermode("/bin/msh");
    ready(init);

	// enable interrupts
	asm("sti");
    sched();

	// become the null process
	while (1)
		asm("hlt");
}
