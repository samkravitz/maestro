#include <string.h>

int strncmp(const char *a, const char *b, size_t n)
{
	const uint8_t *s1 = (const uint8_t *) a;
	const uint8_t *s2 = (const uint8_t *) b;

	while (n && *s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
		n--;
	}

	return n == 0 ? 0 : *s1 - *s2;
}
