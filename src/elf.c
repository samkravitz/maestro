/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: elf.c
 * DATE: April 28th, 2022
 * DESCRIPTION: utilities to read/load elf files
 * RESOURCE: http://www.skyfree.org/linux/references/ELF_Format.pdf
 */
#include <elf.h>

#include <ext2.h>
#include <kmalloc.h>
#include <kprintf.h>
#include <pmm.h>
#include <proc.h>
#include <vfs.h>
#include <vmm.h>

extern struct proc *curr;

extern void enter_usermode(void *, void *);

/**
 * @brief reads, loads, and runs an elf file
 * this function is not called directly, but the starting point
 * of processes that will run in user mode.
 */
void run_elf()
{
    int fd = vfs_open(curr->name);
    int inode = curr->ofile[fd]->n->inode;
    size_t s = ext2_filesize(inode);
	u8 *buff = kmalloc(s);
	ext2_read_data(buff, inode, 0, s);

	struct elf_ehdr *ehdr = (struct elf_ehdr *) buff;
	print_elf(ehdr);

	// Create a new address space for this process
	uintptr_t user_pdir = vmm_create_address_space();
	if (!user_pdir)
	{
		kprintf("Failed to create address space for %s\n", curr->name);
		proc_exit(1);
	}

	// Store the page directory physical address in the process structure
	curr->pdir = user_pdir;

    struct elf_phdr *phdr_table = (struct elf_phdr *) (buff + ehdr->e_phoff);
	struct elf_phdr *phdr;

    phdr = &phdr_table[0];
	for (uint i = 0; i < ehdr->e_phnum; i++)
	{
		phdr = &phdr_table[i];
        for (unsigned j = 0; j <= phdr->p_memsz / PAGE_SIZE; j++)
        {
            uintptr_t phys = pmm_alloc();
            vmm_map_page_in_pdir(curr->pdir, phys, phdr->p_vaddr + j * PAGE_SIZE, PT_PRESENT | PT_WRITABLE | PT_USER);
        }
	}

    // Switch to the process's page directory to copy data into user space
    asm("mov %0, %%cr3" :: "r"(curr->pdir) : "memory");

    // Now copy ELF segments into user space
    for (uint i = 0; i < ehdr->e_phnum; i++)
    {
        phdr = &phdr_table[i];
        memcpy((void *) phdr->p_vaddr, &buff[phdr->p_offset], phdr->p_memsz);
    }

    kfree(buff);

    const char *argv[] = {
        "ls",
        "-lia",
        "arg3",
        NULL,
    };

    void *env = (void *) (0xc0000000 - PR_STACKSIZE);
    uintptr_t env_phys = pmm_alloc();
    vmm_map_page_in_pdir(curr->pdir, env_phys, (uintptr_t) env, PT_PRESENT | PT_WRITABLE | PT_USER);

    const char **arg = argv;
    int argc = 0;
    while (*arg++)
        argc++;


    char **envp = (char **) env;
    unsigned base = (argc + 1) * sizeof(char*);
    for (int i = 0; i < argc; i++)
    {
        memcpy(env + base, argv[i], strlen(argv[i]) + 1);
        envp[i] = env + base;
        base += strlen(argv[i]) + 1;
    }

    envp[argc] = NULL;

    // create user stack and map it
    uintptr_t ustack_phys = pmm_alloc();
    vmm_map_page_in_pdir(curr->pdir, ustack_phys, 0xc0000000 - 2 * PR_STACKSIZE, PT_PRESENT | PT_WRITABLE | PT_USER);
    u32 *ustack = (u32*) (0xc0000000 - PR_STACKSIZE);

    // place argc and argv on the stack
    --ustack; *ustack = (uintptr_t) env;
    --ustack; *ustack = argc;

    enter_usermode(ustack, (void *) ehdr->e_entry);
}

void print_elf(struct elf_ehdr *ehdr)
{
	kprintf("ELF header:\n");
	kprintf("e_ident: %x %c %c %c\n",
	        ehdr->e_ident[0],
	        ehdr->e_ident[1],
	        ehdr->e_ident[2],
	        ehdr->e_ident[3]);    // should be 0x7f E L F

	kprintf("e_type: ");
	switch (ehdr->e_type)
	{
		case ET_NONE:
			kprintf("ET_NONE");
			break;
		case ET_REL:
			kprintf("ET_REL");
			break;
		case ET_EXEC:
			kprintf("ET_EXEC");
			break;
		case ET_CORE:
			kprintf("ET_CORE");
			break;
		default:
			kprintf("UNKNOWN");
	}

	kprintf("(%d)\n", ehdr->e_type);

	kprintf("e_entry: %x\n", ehdr->e_entry);
	kprintf("e_phoff: %x\n", ehdr->e_phoff);
	kprintf("e_shoff: %x\n", ehdr->e_shoff);
	kprintf("e_ehsize: %x\n", ehdr->e_ehsize);
	kprintf("e_phentsize: %x\n", ehdr->e_phentsize);
	kprintf("e_phnum: %x\n", ehdr->e_phnum);
	kprintf("e_shentsize: %x\n", ehdr->e_shentsize);
	kprintf("e_shnum: %x\n", ehdr->e_shnum);
	kprintf("e_shstrndx: %x\n", ehdr->e_shstrndx);
	kprintf("\n");
}
