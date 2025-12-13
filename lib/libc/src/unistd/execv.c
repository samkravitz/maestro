/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/src/unistd/execv.c
 * DATE: December 13, 2025
 * DESCRIPTION: execv library function
 */
#include <syscall.h>
#include <unistd.h>

int execv(const char *path, char *const argv[])
{
	syscall(SYS_EXECV, path, argv);
}
