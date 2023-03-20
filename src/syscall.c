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
#include <kprintf.h>
#include <pmm.h>
#include <proc.h>
#include <vfs.h>
#include <vmm.h>

extern struct proc *curr;

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

/**
 * @brief syscall 1 - write
 * @param fd ebx
 * @param buff ecx
 * @param count edx
 * @return count of bytes actually written
 */
void sys_write(struct registers *regs)
{
	int fd = regs->ebx;
	void *buff = (void *) regs->ecx;
	size_t count = regs->edx;

	regs->eax = vfs_write(fd, buff, count);
}

/**
 * @brief syscall 2 - exit
 * @param status ebx
 * @return none
 */
void sys_exit(struct registers *regs)
{
	int status = regs->ebx;
	proc_exit(status);
}

/**
 * @brief syscall 3 - open
 * @param filename ebx
 * @param flags ecx
 * @param mode edx
 * @return none
 */
void sys_open(struct registers *regs)
{
	char *filename = (char *) regs->ebx;
	regs->eax = vfs_open(filename);
}

/**
 * @brief syscall 4 - sbrk
 * @param increment ebx
 * @return pointer to new sbrk
 */
void sys_sbrk(struct registers *regs)
{
	intptr_t increment = (intptr_t) regs->ebx;
	if (increment == 0)
	{
		regs->eax = (u32) curr->sbrk;
		return;
	}

	void *sbrk = curr->sbrk;

	kassert((increment % PAGE_SIZE) == 0, "sbrk: increment is not a multiple of PAGE_SIZE");
	unsigned num_pages = increment / PAGE_SIZE;
	for (unsigned i = 0; i < num_pages; i++)
	{
		uintptr_t phys = pmm_alloc();
		vmm_map_page(phys, (uintptr_t) curr->sbrk, PT_PRESENT | PT_WRITABLE | PT_USER);
		curr->sbrk += PAGE_SIZE;
	}

	regs->eax = (u32) sbrk;
}

/**
 * @brief syscall 5 - getdents
 * @param fd ebx
 * @param buf ecx
 * @param count edx
 * @return pointer to new sbrk
 */
void sys_getdents(struct registers *regs)
{
	int fd = (int) regs->ebx;
	void *buf = (void *) regs->ecx;
	size_t count = (size_t) regs->edx;

	regs->eax = vfs_readdir(fd, buf, count);
}

/**
 * @brief syscall 6 - fork
 * @return pid
 */
void sys_fork(struct registers *regs)
{
	regs->eax = 0;
}

void (*syscall_handlers[])(struct registers *) = {
	sys_read, sys_write, sys_exit, sys_open, sys_sbrk, sys_getdents, sys_fork,
};

const int NUM_SYSCALLS = sizeof(syscall_handlers) / sizeof(syscall_handlers[0]);
