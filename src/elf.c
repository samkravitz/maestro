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

int execv(const char *pathname, char *const argv[])
{
	size_t s = ext2_filesize(14);
	u8 *buff = kmalloc(s);
	ext2_read_data(buff, 14, 0, s);

	struct elf_ehdr *ehdr = (struct elf_ehdr *) buff;
	print_elf(ehdr);
	//struct elf_phdr *phdr_table = buff + ehdr->e_phoff;
	//struct elf_phdr *phdr;

	//for (uint i = 0; i < ehdr->e_phnum; i++)
	//{
	//	phdr = &phdr_table[i];
	//	kprintf("%d %x %x %x\n", phdr->p_type, phdr->p_offset, phdr->p_vaddr, phdr->p_align);

	//}
	return 0;
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
