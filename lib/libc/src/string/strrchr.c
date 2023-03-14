#include "string.h"

// returns a pointer to the last occurrence of c in str,
// or NULL if it is not found.
// the null terminator is considered part of the string
char *strrchr(const char *str, char c)
{
	size_t i = strlen(str) + 1;
	while (i-- != 0)
	{
		if (str[i] == c)
			return (char *) &str[i];
	}

	return NULL;
}
