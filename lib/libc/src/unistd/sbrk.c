#include <unistd.h>
#include <syscall.h>

void *sbrk(intptr_t increment)
{
    return syscall(SYS_SBRK, increment);
}
