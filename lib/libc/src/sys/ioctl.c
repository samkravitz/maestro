#include <sys/ioctl.h>
#include <syscall.h>

int ioctl(int fd, int op, ...)
{
	return syscall(SYS_IOCTL, fd, op);
}
