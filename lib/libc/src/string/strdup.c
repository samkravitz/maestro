#include <string.h>

char *strdup(const char *str)
{
	return strndup(str, strlen(str));
}
