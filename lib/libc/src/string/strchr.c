#include <string.h>

// returns a pointer to the first occurrence of c in str,
// or NULL if it is not found.
// the null terminator is considered part of the string
char *strchr(const char *str, char c)
{
	for (size_t i = 0; i < strlen(str) + 1; i++)
	{
		if (str[i] == c)
			return (char *) &str[i];
	}

	return NULL;
}
