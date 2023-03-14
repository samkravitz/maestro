#include <string.h>

// sets n bytes of ptr to c
void *memset(void *ptr, int c, size_t n)
{
	char *p = (char *) ptr;

	while (n--)
		*p++ = c;

	return ptr;
}
