/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/syscall.c
 * DATE: April 5th, 2022
 * DESCRIPTION: C library syscall wrappers
 * 
 * Different syscalls have different number arguments, so this wrapper
 * determines how many arguments should appear for each syscall. After
 * unwrapping the arguments, it calls the proper dispatcher in syscall.s
 * to actually call int 48
 */

#include <syscall.h>

#include <stdarg.h>
#include <stdio.h>

/**
 * @brief generic syscall dispatcher
 * @param sysno system call number to dispatch
 * @param args option args for system call
 */
int syscall(int sysno, ...)
{
	uint32_t arg1, arg2, arg3;
	int ret;

	va_list args;
	va_start(args, sysno);

	switch (sysno)
	{
		// syscalls with no arguments
		case SYS_FORK:
			ret = syscall0(sysno);
			break;

		// syscalls with 1 argument
		case SYS_EXIT:
		case SYS_SBRK:
			arg1 = va_arg(args, uint32_t);
			ret = syscall1(sysno, arg1);
			break;

		// syscalls with 2 arguments
		case SYS_EXECV:
			arg1 = va_arg(args, uint32_t);
			arg2 = va_arg(args, uint32_t);
			ret = syscall2(sysno, arg1, arg2);
			break;

		// syscalls with 3 arguments
		case SYS_READ:
		case SYS_WRITE:
		case SYS_OPEN:
		case SYS_GETDENTS:
			arg1 = va_arg(args, uint32_t);
			arg2 = va_arg(args, uint32_t);
			arg3 = va_arg(args, uint32_t);

			ret = syscall3(sysno, arg1, arg2, arg3);
			break;

		default:
			printf("Unknown syscall %d\n", sysno);
	}

	va_end(args);
	return ret;
}
