/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: pit.c
 * DATE: August 2nd, 2021
 * DESCRIPTION: Controllers for the PIT
 */
#include "pit.h"

#include "intr.h"
#include "io.h"
#include "klog.h"

// base frequency of the PIT, in Hz
#define PIT_BASE_RATE 1193180

u64 ticks = 0;

static void handler()
{
	ticks++;
}

// init timer
void itimer()
{
   	// Firstly, register our timer callback.
	regint(IRQ0, handler);

   	// The value we send to the PIT is the value to divide it's input clock
   	// (1193180 Hz) by, to get our required frequency. Important to note is
   	// that the divisor must be small enough to fit into 16-bits.
	u32 divisor = 0xFFFF;//PIT_BASE_RATE / freq_hz;

   	// Send the command byte.
   	outb(0x43, 0x36);

   	// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   	u8 lo = divisor >> 0 & 0xFF;
	u8 hi = divisor >> 8 & 0xFF;

	// Send the frequency divisor.
	outb(0x40, lo);
	outb(0x40, hi);
}