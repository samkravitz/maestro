/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/unistd.c
 * DATE: April 6th, 2022
 * DESCRIPTION: POSIX compatibility definitions
 */

#include "unistd.h"

#include "stdio.h"
#include "syscall.h"

size_t read(int fd, void *buff, size_t count)
{
	return syscall(SYS_READ, fd, buff, count);
}

size_t write(int fd, void *buff, size_t count)
{
	return syscall(SYS_WRITE, fd, buff, count);
}

void *sbrk(intptr_t increment)
{
	return NULL;
}
