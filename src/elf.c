/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: elf.c
 * DATE: April 28th, 2022
 * DESCRIPTION: utilities to read/load elf files
 */
#include <elf.h>

#include <ext2.h>
#include <kprintf.h>
#include <kmalloc.h>
#include <pmm.h>
#include <proc.h>
#include <vfs.h>
#include <vmm.h>

extern struct proc *curr;

int execv(const char *pathname, char *const argv[])
{
	//size_t s = ext2_filesize(14);
	//u8 *buff = kmalloc(s);
	//ext2_read_data(buff, 14, 0, s);
	//kprintf("%x %c %c %c\n", buff[0], buff[1], buff[2], buff[3]);

	//struct elf_ehdr *ehdr = (struct elf_ehdr *) buff;
	//struct elf_phdr *phdr_table = buff + ehdr->e_phoff;
	//struct elf_phdr *phdr;

	//for (uint i = 0; i < ehdr->e_phnum; i++)
	//{
	//	phdr = &phdr_table[i];
	//	kprintf("%d %x %x %x\n", phdr->p_type, phdr->p_offset, phdr->p_vaddr, phdr->p_align);
		
	//}
	return 0;
}
