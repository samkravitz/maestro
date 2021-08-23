/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: clk.c
 * DATE: August 2nd, 2021
 * DESCRIPTION: Controllers for the PIT
 */
#include <clk.h>

#include <intr.h>
#include <io.h>
#include <kout.h>
#include <proc.h>

// base frequency of the PIT, in Hz
#define PIT_BASE_RATE 	1193180

static u64 sec = 0; 	// seconds since maestro was bootstrapped
static int ms  = 0; 	// ms since sec was last updated

static void clkhandler()
{
	if (++ms == 1000)
	{
		++sec;
		ms = 0;
		// sched();
	}
}

// init clk
void clkinit()
{
	svect(IRQ0, clkhandler);

	// we want our clock interrupt to trigger at a rate of 1000Hz,
	// or once every 1ms. Hence we divide by 1000.
	u32 divisor = PIT_BASE_RATE / 1000;

	// Send the command byte.
	outb(0x43, 0x36);

	// Send the frequency divisor.
	outb(0x40, divisor >> 0 & 0xFF);
	outb(0x40, divisor >> 8 & 0xFF);
}
