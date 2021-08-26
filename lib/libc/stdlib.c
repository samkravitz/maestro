#include "stdlib.h"

static void rev(char *, int);

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
char *itoa(u32 n, char *buff, int base)
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