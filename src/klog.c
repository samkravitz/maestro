/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: klog.c
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#include <klog.h>

#include <maestro/stdlib.h>
#include <maestro/string.h>
#include <tty.h>

// general logging function
void klog(const char *msg)
{
	puts(msg);
}

// logs a base 10 integer
void klogd(int x)
{
	char buff[32];
	memset(buff, 0, sizeof(buff));
	itoa(x, buff, 10);
	klog(buff);
}

// logs a hexadecimal integer
void klogh(int x)
{
	char buff[32];
	memset(buff, 0, sizeof(buff));
	itoa(x, buff, 16);
	klog(buff);
}

// like printf, but better...
int kprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int x, i = 0;
	char buff[256] = {0}; // note - kprintf can print a max length of 256
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
				case 'd':
					x = va_arg(args, int);
					itoa(x, fmtbuf, 10);
					strcat(buff, fmtbuf);
					i += strlen(fmtbuf);
					fmt += 2;
					break;

				// base 16 integer
				case 'x':
					u32 px = va_arg(args, u32);
					itoa(px, fmtbuf, 16);
					strcat(buff, fmtbuf);
					i += strlen(fmtbuf);
					fmt += 2;
					break;
				
				// string
				case 's':
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
	klog(buff);
	return i;
}