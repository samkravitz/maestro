#include "unistd.h"

void *sbrk(intptr_t increment)
{
    (void) increment;
	return NULL;
}
