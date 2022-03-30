/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: init.c
 * DATE: August 1st, 2021
 * DESCRIPTION: Initializes structures and systems vital for kernel
 */
#include <init.h>

#include <clk.h>
#include <ext2.h>
#include <idt.h>
#include <intr.h>
#include <kbd.h>
#include <pmm.h>
#include <proc.h>
#include <vfs.h>
#include <vmm.h>

// initializes IDT, interrupts, and the clock
void init()
{
	intr_init();
	idt_init();
	clk_init();
	pmm_init();
	vmm_init();

	ext2_init();
    vfs_init();

	proc_init();

	// set keyboard interrupt handler
	set_vect(IRQ1, kbdhandler);
}
