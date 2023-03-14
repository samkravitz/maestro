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
