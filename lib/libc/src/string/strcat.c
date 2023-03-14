#include "string.h"

char *strcat(char *dest, const char *src)
{
	size_t i = 0, len = strlen(dest);

	while (src[i])
	{
		dest[i + len] = src[i];
		i++;
	}

	dest[i + len] = '\0';

	return dest;
}
