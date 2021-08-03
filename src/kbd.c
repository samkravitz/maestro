/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kbd.c
 * DATE: August 2nd, 2021
 * DESCRIPTION: Driver for PS/2 keyboard
 */
#include <kbd.h>

#include <intr.h>
#include <io.h>
#include <klog.h>

static void handler()
{
	u8 key = inb(0x60);
	if (key < 0x58 && keylut[key])
		kprintf("%c", keylut[key]);
}

void kbd_init()
{
	// register keyboard interrupt handler
	regint(IRQ1, handler);
}
