#include <dirent.h>
#include <stddef.h>
#include <syscall.h>

struct dirent *readdir(DIR *dir)
{
	struct dirent *de;

	if (dir->buf_pos == 0)
	{
		int len = syscall(SYS_GETDENTS, dir->fd, dir->buf, sizeof(dir->buf));

		if (len < 0)
			return NULL;

		dir->buf_end = len;
	}

	de = (struct dirent *) (dir->buf + dir->buf_pos);
	if (dir->buf_pos >= dir->buf_end)
		return NULL;

	dir->buf_pos += de->d_reclen;
	return de;
}
