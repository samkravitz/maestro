#include <unistd.h>
#include <syscall.h>

void exit(int status)
{
    syscall(SYS_EXIT, status);
}
