/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kout.c
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#include <kout.h>

#include <intr.h>
#include <tty.h>

#include "stdlib.h"
#include "string.h"

// general logging function
void kout(const char *msg)
{
	puts(msg);
}

// like printf, but better...
int koutf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int x, i = 0;
	char buff[256] = {0}; // note - koutf can print a max length of 256
	char c;

	char fmtbuf[32];

	while ((c = *fmt) != '\0')
	{
		// check for a format specifier
		if (c == '%')
		{
			// clear format buffer 
			memset(fmtbuf, 0, sizeof(fmtbuf));

			// check next character
			switch (*(fmt + 1))
			{
				// character
				case 'c':
					x = va_arg(args, int);
					buff[i] = (char) x;
					i++;
					fmt += 2;
					break;
				
				// base 10 integer
				case 'd': ;
					x = va_arg(args, int);
					itoa(x, fmtbuf, 10);
					strcat(buff, fmtbuf);
					i += strlen(fmtbuf);
					fmt += 2;
					break;

				// base 16 integer
				case 'x': ;
					u32 px = va_arg(args, u32);
					itoa(px, fmtbuf, 16);
					strcat(buff, fmtbuf);
					i += strlen(fmtbuf);
					fmt += 2;
					break;
				
				// string
				case 's': ;
					char *str = va_arg(args, char *);
					strcat(buff, str);
					i += strlen(str);
					fmt += 2;
					break;

				// actual '%' character
				default:
					buff[i] = '%';
					i++;
					fmt++;		
			}
		}

		// non-format character
		else
		{
			buff[i++] = c;
			fmt++;
		}

	}

	va_end(args);
	kout(buff);
	return i;
}