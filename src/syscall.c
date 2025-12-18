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

#include <elf.h>
#include <ext2.h>
#include <intr.h>
#include <kmalloc.h>
#include <kprintf.h>
#include <pmm.h>
#include <proc.h>
#include <vfs.h>
#include <vmm.h>

// defined in proc.c
extern struct proc *curr;
extern struct proc *proctab[];

extern void enter_usermode(void *, void *);

/**
 * @brief syscall 0 - read
 * @param fd ebx
 * @param buff ecx
 * @param count edx
 * @return count of bytes actually read
 */
static void sys_read(struct registers *regs)
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
static void sys_write(struct registers *regs)
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
static void sys_exit(struct registers *regs)
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
static void sys_open(struct registers *regs)
{
	char *filename = (char *) regs->ebx;
	regs->eax = vfs_open(filename);
}

/**
 * @brief syscall 4 - sbrk
 * @param increment ebx
 * @return pointer to new sbrk
 */
static void sys_sbrk(struct registers *regs)
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
static void sys_getdents(struct registers *regs)
{
	int fd = (int) regs->ebx;
	void *buf = (void *) regs->ecx;
	size_t count = (size_t) regs->edx;

	regs->eax = vfs_readdir(fd, buf, count);
}

/**
 * @brief syscall 6 - fork
 * @return child pid to parent, 0 to child
 */
static void sys_fork(struct registers *regs)
{
	regs->eax = proc_fork(regs);
}

/**
 * @brief syscall 7 - execv
 * @param path ebx
 * @param argv ecx
 * @return does not return on success, -1 on error
 */
static void sys_execv(struct registers *regs)
{
	const char *path = (const char *) regs->ebx;
	char **argv = (char **) regs->ecx;

	// Copy path to kernel memory before we destroy address space
	char kpath[32];
	unsigned i;
	for (i = 0; i < sizeof(kpath) - 1 && path[i] != '\0'; i++)
		kpath[i] = path[i];
	kpath[i] = '\0';

	// Count argc and copy argv strings to kernel memory
	int argc = 0;
	char *kargv[16];    // max 16 arguments
	while (argv && argv[argc] != NULL && argc < 16)
	{
		// Get length of argument string
		const char *arg = argv[argc];
		size_t len = 0;
		while (arg[len] != '\0')
			len++;

		// Allocate and copy string
		kargv[argc] = kmalloc(len + 1);
		for (size_t j = 0; j <= len; j++)
			kargv[argc][j] = arg[j];

		argc++;
	}

	// Open and read ELF file
	int fd = vfs_open(kpath);
	if (fd < 0)
	{
		// Cleanup and return error
		for (int i = 0; i < argc; i++)
			kfree(kargv[i]);
		regs->eax = -1;
		return;
	}

	int inode = curr->ofile[fd]->n->inode;
	size_t s = ext2_filesize(inode);
	u8 *buff = kmalloc(s);
	ext2_read_data(buff, inode, 0, s);

	struct elf_ehdr *ehdr = (struct elf_ehdr *) buff;

	// Create new address space
	uintptr_t user_pdir = vmm_create_address_space();
	if (!user_pdir)
	{
		kfree(buff);
		for (int i = 0; i < argc; i++)
			kfree(kargv[i]);
		regs->eax = -1;
		return;
	}

	// Update process state
	curr->pdir = user_pdir;
	for (i = 0; i < sizeof(curr->name) - 1 && kpath[i] != '\0'; i++)
		curr->name[i] = kpath[i];
	curr->name[i] = '\0';
	curr->sbrk = NULL;    // Reset heap

	// Map ELF program segments (only PT_LOAD segments)
	struct elf_phdr *phdr_table = (struct elf_phdr *) (buff + ehdr->e_phoff);
	for (uint i = 0; i < ehdr->e_phnum; i++)
	{
		struct elf_phdr *phdr = &phdr_table[i];
		if (phdr->p_type != PT_LOAD)
			continue;
		for (unsigned j = 0; j <= phdr->p_memsz / PAGE_SIZE; j++)
		{
			uintptr_t phys = pmm_alloc();
			vmm_map_page_in_pdir(curr->pdir, phys, phdr->p_vaddr + j * PAGE_SIZE, PT_PRESENT | PT_WRITABLE | PT_USER);
		}
	}

	// Switch to the new address space
	asm("mov %0, %%cr3" ::"r"(curr->pdir) : "memory");

	// Copy ELF segments into user space (only PT_LOAD segments)
	for (uint i = 0; i < ehdr->e_phnum; i++)
	{
		struct elf_phdr *phdr = &phdr_table[i];
		if (phdr->p_type != PT_LOAD)
			continue;
		char *dst = (char *) phdr->p_vaddr;
		char *src = (char *) &buff[phdr->p_offset];
		for (size_t j = 0; j < phdr->p_memsz; j++)
			dst[j] = src[j];
	}

	kfree(buff);

	// Set up environment page for argv
	void *env = (void *) (0xc0000000 - PR_STACKSIZE);
	uintptr_t env_phys = pmm_alloc();
	vmm_map_page_in_pdir(curr->pdir, env_phys, (uintptr_t) env, PT_PRESENT | PT_WRITABLE | PT_USER);

	// Copy argv strings to environment page
	char **envp = (char **) env;
	unsigned base = (argc + 1) * sizeof(char *);
	for (int i = 0; i < argc; i++)
	{
		// Copy string to environment page
		char *dst = (char *) env + base;
		char *src = kargv[i];
		size_t len = 0;
		while (src[len] != '\0')
		{
			dst[len] = src[len];
			len++;
		}
		dst[len] = '\0';

		// Set pointer in envp array
		envp[i] = dst;
		base += len + 1;

		// Free kernel copy
		kfree(kargv[i]);
	}
	envp[argc] = NULL;

	// Create user stack
	uintptr_t ustack_phys = pmm_alloc();
	vmm_map_page_in_pdir(curr->pdir, ustack_phys, 0xc0000000 - 2 * PR_STACKSIZE, PT_PRESENT | PT_WRITABLE | PT_USER);
	u32 *ustack = (u32 *) (0xc0000000 - PR_STACKSIZE);

	// Push argc and argv onto stack
	--ustack;
	*ustack = (uintptr_t) env;
	--ustack;
	*ustack = argc;

	// Jump to new program entry point (does not return)
	enter_usermode(ustack, (void *) ehdr->e_entry);
}

/**
 * @brief syscall 8 - close
 * @param fd ebx
 * @return 0 on success, -1 on error
 */
static void sys_close(struct registers *regs)
{
	int fd = (int) regs->ebx;
	regs->eax = vfs_close(fd);
}

/**
 * @brief syscall 9 - getenv
 * @param name ebx
 * @return pointer to environment variable value, or NULL if not found
 */
static void sys_getenv(struct registers *regs)
{
	const char *name = (const char *) regs->ebx;
	const size_t len = strlen(name);

	char **argv = (char **) curr->ustack;
	char **envp = &argv[0];

	// skip past all argv entries
	while (*envp)
		envp++;

	// skip the NULL itself to get to the start of envp
	envp++;

	while (*envp)
	{
		char *entry = *envp;

		// find '=' in entry
		char *eq = entry;
		while (*eq != '=' && *eq != '\0')
			eq++;

		// compare name with env variable name
		size_t name_len = eq - entry;
		if (strncmp(name, entry, len) == 0 && name[name_len] == '\0')
		{
			// Found matching variable, return pointer to value
			regs->eax = (u32) (eq + 1);
			return;
		}

		envp++;
	}

	regs->eax = 0;
}

/**
 * @brief syscall 10 - waitpid
 * @param pid ebx
 * @param status ecx
 * @param options edx
 * @return pid of terminated child, -1 on error
 */
static void sys_waitpid(struct registers *regs)
{
	int pid = (int) regs->ebx;
	int *status = (int *) regs->ecx;
	int options = (int) regs->edx;

	struct proc *child = proctab[pid];
	if (child == NULL || child->parent != curr->pid)
	{
		kprintf("waitpid: no such child process %d\n", pid);
		regs->eax = -1;
		return;
	}

	// If child is already terminated, return immediately
	if (child->state == PR_TERMINATED)
	{
		if (status)
			*status = 0;    // TODO - set actual exit status
		regs->eax = pid;
		return;
	}

	// Otherwise, put current process to sleep until child terminates
	curr->state = PR_WAITING;
	curr->waiting_for = pid;
	sched();
}

void (*syscall_handlers[])(struct registers *) = { sys_read, sys_write, sys_exit,  sys_open,   sys_sbrk,   sys_getdents,
	                                               sys_fork, sys_execv, sys_close, sys_getenv, sys_waitpid };

const int NUM_SYSCALLS = sizeof(syscall_handlers) / sizeof(syscall_handlers[0]);
