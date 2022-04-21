/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: mouse.c
 * DATE: April 20th, 2022
 * DESCRIPTION: PS/2 mouse driver
 */
#include <mouse.h>

#include <kprintf.h>
#include <intr.h>
#include <io.h>

/**
 * before sending output to port 0x60 or 0x64, 
 * wait until bit 1 of port 0x64 is clear
 * 
 * before receiving input from port 0x60, 
 * wait until bit 0 of port 0x64 is set
 * 
 * @param type either WAIT_IN or WAIT_OUT before input or output respectively
 * 
 */
static void wait(int type)
{
	if (type == WAIT_IN)
	{
		while (!(inb(MOUSE_STATUS) & 1))
			;
	}

	else if (type == WAIT_OUT)
	{
		while (inb(MOUSE_STATUS) & 2)
			;
	}
	
	else
	{
		kprintf("Invalid mouse wait: %d\n", type);
	}
}

static void mouse_handler()
{
	wait(WAIT_IN);
	u8 a = inb(MOUSE_DATA);
	wait(WAIT_IN);
	u8 b = inb(MOUSE_DATA);
	wait(WAIT_IN);
	u8 c = inb(MOUSE_DATA);
	kprintf("mouse handler: %x %x %x\n", a, b, c);
}

void mouse_init()
{
	wait(WAIT_OUT);
	outb(MOUSE_STATUS, 0x20);

	wait(WAIT_IN);
	u8 status = inb(MOUSE_DATA);
	status |= 2;
	status &= 0b11011111;

	wait(WAIT_OUT);
	outb(MOUSE_STATUS, 0x60);

	wait(WAIT_OUT);
	outb(MOUSE_DATA, status);

	wait(WAIT_OUT);
	outb(MOUSE_STATUS, 0xa8);

	wait(WAIT_OUT);
	outb(MOUSE_STATUS, MOUSE_CMD);
	wait(WAIT_OUT);
	outb(MOUSE_DATA, 0xf6);

	wait(WAIT_OUT);
	outb(MOUSE_STATUS, MOUSE_CMD);
	wait(WAIT_OUT);
	outb(MOUSE_DATA, 0xf4);
	
	set_vect(IRQ12, mouse_handler);
}
