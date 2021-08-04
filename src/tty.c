/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: tty.c
 * DATE: July 26, 2021
 * DESCRIPTION: Functions for writing to the VGA text mode terminalS
 */
#include "tty.h"

#include "io.h"

#define TTY_WIDTH 80
#define TTY_HEIGHT 25

// attribute byte for white text on black background
#define ATTRIBUTE (0xF << 8)

// get color
#define GETCOL(c) (c | ATTRIBUTE)

// coordinate of cursor
static u8 x = 0;
static u8 y = 0;

// vga base address
u16 *VGA_BASE = (u16 *)0xB8000;

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

	// shift all rows up one
	int idx = 0;
	int max = TTY_WIDTH * TTY_HEIGHT - TTY_WIDTH;
	while (idx < max)
	{
		VGA_BASE[idx] = VGA_BASE[idx + TTY_WIDTH];
		idx++;
	}

	// fill bottom row with spaces
	for (int i = 0; i < TTY_WIDTH; ++i)
		VGA_BASE[i + max] = GETCOL(' ');

	y = 24;
}

// set cursor
static void setcur()
{
	u16 pos = y * TTY_WIDTH + x;
	outb(0x3D4, 0xE);
	outb(0x3D5, pos >> 8);
	outb(0x3D4, 0xF);
	outb(0x3D5, pos);
}