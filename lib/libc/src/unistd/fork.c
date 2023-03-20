#include <syscall.h>
#include <unistd.h>

pid_t fork()
{
	pid_t pid = (pid_t) syscall(SYS_FORK);
	return pid;
}
