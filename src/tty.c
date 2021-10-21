/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: tty.c
 * DATE: July 26, 2021
 * DESCRIPTION: Functions for writing to the VGA text mode terminalS
 */
#include <tty.h>

#include <intr.h>
#include <io.h>
#include "string.h"

#define TTY_WIDTH 		80
#define TTY_HEIGHT		25

// attribute byte for white text on black background
#define ATTRIBUTE		(0xf << 8)

// get color
#define GETCOL(c)		(c | ATTRIBUTE)

// coordinate of cursor
static u8 x = 0;
static u8 y = 0;

// vga base address
// this is the virtual address of 0xb8000 mapped to kernel page 1023
u16 *VGA_BASE = (u16 *) (uint16_t*) 0xc03ff000;

static void scroll();
static void setcur();

void putc(char c)
{
	// handle special characters
	switch (c)
	{
		case '\n':
			y++;
			x = 0;
			break;

		case '\b':
			if (x > 0)
				x--;

			u16 index = y * TTY_WIDTH + x;
			VGA_BASE[index] = GETCOL(' ');
			break;

		case '\r':
			x = 0;
			break;

		default:
			// is a printable char
			if (c >= 32)
			{
				u16 index = y * TTY_WIDTH + x;
				VGA_BASE[index] = GETCOL(c);
				x++;
			}
	}

	if (x == TTY_WIDTH - 1)
	{
		y++;
		x = 0;
	}

	scroll();
	setcur();
}

void puts(const char *str)
{
	while (*str)
		putc(*str++);
}

// clear - clears the terminal
void clear()
{
	for (int i = 0; i < TTY_WIDTH * TTY_HEIGHT; ++i)
		putc(' ');
}

// scroll screen, if necessary
static void scroll()
{
	if (y < 25)
		return;

	// index of last row in vga memory
	const int lrow = TTY_WIDTH * TTY_HEIGHT - TTY_WIDTH;

	// shift all rows up one
	memmove(VGA_BASE, &VGA_BASE[TTY_WIDTH], lrow * 2);

	// fill bottom row with spaces
	for (int i = 0; i < TTY_WIDTH; ++i)
		VGA_BASE[i + lrow] = GETCOL(' ');

	y = 24;
}

// set cursor
static void setcur()
{
	u16 pos = y * TTY_WIDTH + x;
	outb(0x3D4, 0xe);
	outb(0x3D5, pos >> 8);
	outb(0x3D4, 0xf);
	outb(0x3D5, pos);
}