#include "stdlib.h"

static void rev(char *, int);

// converts a number into a string
char *itoa(int n, char *buff, int base)
{
	char c;
	int mod, i = 0;

	int neg = 0;
	if ((base == 10) && (n < 0))
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

	rev(buff, i);
	return buff;
}

// reverses a string of a given length
static void rev(char *str, int len)
{
	if (len <= 0)
		return;

	char temp[len];

	for (int i = 0; i < len; ++i)
		temp[i] = str[len - i - 1];

	for (int i = 0; i < len; ++i)
		str[i] = temp[i];
}