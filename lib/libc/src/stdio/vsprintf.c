#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int vsprintf(char *str, const char *fmt, va_list args)
{
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
					str[i++] = (char) x;
					fmt += 2;
					break;

				// base 10 integer
				case 'd':
					x = va_arg(args, int);
					itoa(x, fmtbuf, 10);
					fmt += 2;
					break;

				// base 16 integer
				case 'x':
					x = va_arg(args, uint32_t);
					itoa(x, fmtbuf, 16);
					fmt += 2;
					break;

				// string
				case 's':
					char *s = va_arg(args, char *);
					strcat(&str[i], s);
					i += strlen(s);
					fmt += 2;
					break;

				// actual '%' character
				default:
					str[i++] = '%';
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

			strcat(&str[i], fmtbuf);
			i += strlen(fmtbuf);
		}

		// non-format character
		else
		{
			str[i++] = c;
			fmt++;
		}
	}

	return i;
}
