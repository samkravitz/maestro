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

#include "syscall.h"

#include <stdarg.h>

/**
 * @brief generic syscall dispatcher
 * @param sysno system call number to dispatch
 * @param args option args for system call
 */
int syscall(int sysno, ...)
{
	u32 arg1, arg2, arg3;
	int ret;

	va_list args;
	va_start(args, sysno);

	switch (sysno)
	{
		// syscalls with 3 arguments
		case SYS_READ:
			arg1 = va_arg(args, u32);
			arg2 = va_arg(args, u32);
			arg3 = va_arg(args, u32);

			ret = syscall3(sysno, arg1, arg2, arg3);
			break;

		default:
			;
	}

	va_end(args);
	return ret;
}
