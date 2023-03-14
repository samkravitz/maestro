#include <string.h>

char *strtok(char *str, const char *delim)
{
	// buffer to persist str between calls
	static char *save;

	if (!str)
		str = save;

	if (!str || strlen(str) == 0)
		return NULL;

	size_t delim_len = strlen(delim);

	// check if str begins with delim
	if (strncmp(str, delim, delim_len) == 0)
	{
		str += delim_len;
		if (!str)
			return NULL;
	}

	char *ret = str;

	for (; *str; str++)
	{
		if (strncmp(str, delim, delim_len) == 0)
		{
			*str = '\0';
			save = str + 1;
			return ret;
		}
	}

	save = NULL;
	return ret;
}
