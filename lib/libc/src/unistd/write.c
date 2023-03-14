#include <unistd.h>
#include <syscall.h>

size_t write(int fd, void *buff, size_t count)
{
	return syscall(SYS_WRITE, fd, buff, count);
}
