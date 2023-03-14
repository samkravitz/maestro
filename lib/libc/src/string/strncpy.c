#include "string.h"

char *strncpy(char *dest, const char *src, size_t n)
{
	memcpy(dest, src, n);
	return dest;
}
