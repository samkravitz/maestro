#include "unistd.h"
#include "syscall.h"

size_t read(int fd, void *buff, size_t count)
{
	return syscall(SYS_READ, fd, buff, count);
}
