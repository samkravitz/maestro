#include <unistd.h>
#include <syscall.h>

void *sbrk(intptr_t increment)
{
    return (void*) syscall(SYS_SBRK, increment);
}
