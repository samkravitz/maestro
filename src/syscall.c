/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: syscall.c
 * DATE: April 5th, 2022
 * DESCRIPTION: System call handlers
 * 
 * When these functions are called, the process will already be put in
 * kernel mode by the isr(). So, all of the kernel's utilities are
 * available to use here. Arguments are optional and passed through
 * registers: arg1 in ebx, arg2 in ecx, and arg3 in edx. These registers
 * are accessible through the regs argument passed to each function.
 * 
 * Place the return value that will be passed back to the C library
 * wrapper in eax
 */

#include <syscall.h>

#include <intr.h>
#include <vfs.h>

/**
 * @brief syscall 0 - read
 * @param fd ebx
 * @param buff ecx
 * @param count edx
 * @return count of bytes actually read
 */
void sys_read(struct registers *regs)
{
	int fd = regs->ebx;
	void *buff = (void *) regs->ecx;
	size_t count = regs->edx;

	regs->eax = vfs_read(fd, buff, count);
}

void (*syscall_handlers[])(struct registers *) = {
	sys_read,
};

const int NUM_SYSCALLS = sizeof(syscall_handlers) / sizeof(syscall_handlers[0]);
