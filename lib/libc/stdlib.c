#include "stdlib.h"
#include "string.h"

// converts a string into a number
int atoi(const char *str)
{
	int neg = 0;
	if (*str == '-')
	{
		neg = 1;
		str++;
	}

	int num = 0;

	while (*str && is_numeric(*str))
	{
		num += *str - '0';
		str++;
		num *= 10;
	}

	// account for multiplying by 10 one too many times
	num /= 10;

	if (neg)
		num *= -1;

	return num;
}

// converts a number into a string
char *itoa(uint32_t n, char *buff, int base)
{
	char c;
	int mod, i = 0;

	int neg = 0;
	if ((base == 10) && ((int) n < 0))
	{
		neg = 1;
		n *= -1;
	}

	do
	{
		mod = n % base;

		if (mod >= 10)
			c = 'a' + (mod - 10);

		else
			c = '0' + mod;

		buff[i++] = c;
		n /= base;
	} while (n);

	if (neg)
		buff[i++] = '-';

	return strrev(buff);
}
