#include <fcntl.h>
#include <syscall.h>
#include <stdarg.h>

int open(const char *filename, int flags, ...)
{
    int mode = 0;

    if (flags & O_CREAT)
    {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, int);
        va_end(ap);
    }

    int fd = syscall(SYS_OPEN, filename, flags, mode);
    return fd;
}