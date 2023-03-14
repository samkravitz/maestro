#include <stdio.h>
#include <unistd.h>

#ifndef LIBK
int getc()
{
	char c;
	read(STDIN_FILENO, &c, 1);
	return c;
}
#endif
