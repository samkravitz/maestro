/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kprintf.c
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#include <kprintf.h>

#include <io.h>

#include <stdio.h>

// COM1 serial port
#define COM1 0x3f8

static void serial_write(const char *s);
static void serial_putc(char c);

void kputc(char c)
{
	serial_putc(c);
}

void kputs(const char *msg)
{
	serial_write(msg);
}

int vkprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buff[1024];
	memset(buff, 0, 1024);
	int ret = vsprintf(buff, fmt, args);
	va_end(args);
	serial_write(buff);
	return ret;
}

static void serial_write(const char *s)
{
	while (*s)
		serial_putc(*s++);
}

static void serial_putc(char c)
{
	// one-time initialization of  serial port
	// taken from serenityOS: https://github.com/SerenityOS/serenity/blob/master/Kernel/kprintf.cpp
	static bool serial_initialized = false;
	if (!serial_initialized)
	{
		outb(COM1 + 1, 0x00);
		outb(COM1 + 3, 0x80);
		outb(COM1 + 0, 0x02);
		outb(COM1 + 1, 0x00);
		outb(COM1 + 3, 0x03);
		outb(COM1 + 2, 0xc7);
		outb(COM1 + 4, 0x0b);

		serial_initialized = true;
	}

	// wait until serial port is ready
	while ((inb(COM1 + 5) & 0x20) == 0)
		;

	// output character to serial port
	outb(COM1, c);
}
