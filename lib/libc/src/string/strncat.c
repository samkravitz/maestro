#include <string.h>

char *strncat(char *dest, const char *src, size_t n)
{
	size_t i = 0, len = strlen(dest);

	while (src[i] && n--)
	{
		dest[i + len] = src[i];
		i++;
	}

	dest[i + len] = '\0';

	return dest;
}
