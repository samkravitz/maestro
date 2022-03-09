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
#include <fs.h>
#include <idt.h>
#include <intr.h>
#include <kbd.h>
#include <pmm.h>

// initializes IDT, interrupts, and the clock
void init()
{
	idtinit();
	intrinit();
	clkinit();
	pmminit();

	mount();

	// set keyboard interrupt handler
	svect(IRQ1, kbdhandler);
}
