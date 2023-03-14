#include "stdio.h"
#include "string.h"
#include "unistd.h"

#define PRINTF_BUFF_SIZE 1024

#ifndef LIBK
int vprintf(const char *fmt, va_list args)
{
	char buff[PRINTF_BUFF_SIZE];
	memset(buff, 0, PRINTF_BUFF_SIZE);
	int ret = vsprintf(buff, fmt, args);
	write(STDOUT_FILENO, buff, ret);
	return ret;
}
#endif
