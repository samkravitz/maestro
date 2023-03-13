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

#include "stdio.h"
#include "string.h"

extern struct proc *curr;
extern struct proc nullproc;
extern void start_proc();

void kmain()
{
	kprintf("Welcome to maestro!\n");
	init();
	curr = &nullproc;

	//char *const argv[] = {
	//	"ls",
	//	NULL,
	//};

	//execv("/ls", argv);

    struct proc *ls = create(start_proc, "ls");
    ready(ls);

	// enable interrupts
	asm("sti");

	// become the null process
	while (1)
		asm("hlt");
}
