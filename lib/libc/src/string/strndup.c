#include "string.h"
#include "stdlib.h"

#ifdef LIBK
#include <kmalloc.h>
#undef malloc
#define malloc kmalloc
#endif

char *strndup(const char *str, size_t n)
{
	char *dup = (char *) malloc(n + 1);
	memcpy(dup, str, n);
	dup[n] = '\0';
	return dup;
}
