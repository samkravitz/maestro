#include "string.h"

int strcmp(const char *a, const char *b)
{
	return strncmp(a, b, strlen(a));
}
