#include <sys/wait.h>
#include <syscall.h>

int waitpid(int pid, int *status, int options)
{
	return syscall(SYS_WAITPID, pid, status, options);
}
