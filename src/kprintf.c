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

#include <tty.h>

#include "stdlib.h"
#include "string.h"

void kputc(char c)
{
    putc(c);
}

// general logging function
void kputs(const char *msg)
{
	puts(msg);
}

int kprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int x, i = 0;
	char c;

	char fmtbuf[32];

	while ((c = *fmt) != '\0')
	{
		// check for a format specifier
		if (c == '%')
		{
			// clear format buffer
			memset(fmtbuf, 0, sizeof(fmtbuf));

			// min width of format
			size_t width = 0;
            
            // width specifier: e.x. %5d
			if (is_numeric(*(fmt + 1)))
			{
				width = atoi(fmt + 1);
				fmt++;
			}

			// check next character
			switch (*(fmt + 1))
			{
				// character
				case 'c':
					x = va_arg(args, int);
                    kputc((char) x);
					i++;
					fmt += 2;
					break;

				// base 10 integer
				case 'd':
					x = va_arg(args, int);
					itoa(x, fmtbuf, 10);
					i += strlen(fmtbuf);
					fmt += 2;
					break;

				// base 16 integer
				case 'x':
					x = va_arg(args, u32);
					itoa(x, fmtbuf, 16);
					i += strlen(fmtbuf);
					fmt += 2;
					break;

				// string
				case 's':;
					char *str = va_arg(args, char *);
					kputs(str);
					i += strlen(str);
					fmt += 2;
					break;

				// actual '%' character
				default:
                    kputc('%');
					i++;
					fmt++;
			}

            // the format string did not reach its minimum width, so pad it
            if (strlen(fmtbuf) < width)
            {
                // number of bytes we neet to pad
                size_t diff = width - strlen(fmtbuf);
                memmove(fmtbuf + diff, fmtbuf, strlen(fmtbuf));

                // TODO - for numbers, the default is to pad with '0',
                // for strings, the default is to pad with ' '
                while (diff-- != 0)
                    fmtbuf[diff] = '0';
            }

            kputs(fmtbuf);
		}

		// non-format character
		else
		{
            kputc(c);
            i++;
			fmt++;
		}
	}

	va_end(args);
	return i;
}
