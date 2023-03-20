#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <syscall.h>

DIR *opendir(const char *name)
{
	int fd;
	DIR *dir;

	if ((fd = open(name, O_RDONLY | O_DIRECTORY | O_CLOEXEC)) < 0)
		return NULL;

	if (!(dir = calloc(1, sizeof *dir)))
	{
		return NULL;
	}

	dir->fd = fd;
	return dir;
}
