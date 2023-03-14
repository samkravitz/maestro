#include "string.h"

// copy memory area
void *memmove(void *dest, const void *src, size_t n)
{
	char *d = (char *) dest;
	const char *s = (const char *) src;

	// copy from end so the buffer's don't overwrite one another
	if ((uintptr_t) src < (uintptr_t) dest)
	{
		d += n - 1;
		s += n - 1;
		while (n--)
			*d-- = *s--;
	}

	// copy from beginning (basically just memcpy)
	else
	{
		while (n--)
			*d++ = *s++;
	}

	return dest;
}
