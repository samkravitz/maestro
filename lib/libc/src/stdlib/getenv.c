#include <stdlib.h>
#include <syscall.h>

char *getenv(const char *name)
{
	if (!name)
		return NULL;

	return (char *) syscall(SYS_GETENV, name);
}
