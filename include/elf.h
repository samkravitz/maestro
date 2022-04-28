/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: elf.h
 * DATE: April 28th, 2022
 * DESCRIPTION: utilities to read/load elf files
 */
#ifndef ELF_H
#define ELF_H

#include <maestro.h>

struct elf_ehdr
{
	u8 e_ident[16];
	u16 e_type;
	u16 e_machine;
	u32 e_version;
	u32 e_entry;        // virtual address at which system will transfer control
	u32 e_phoff;        // program header table's offset in bytes
	u32 e_shoff;        // section header table's offset in bytes
	u32 e_flags;
	u16 e_ehsize;       // elf header's size in bytes
	u16 e_phentsize;    // size in bytes of one entry of program header table
	u16 e_phnum;        // number of entries in the program header table
	u16 e_shentsize;    // size in bytes of one entry of section header table
	u16 e_shnum;        // number of entries in the section header
	u16 e_shstrndx;     // index into section header table of the entry associated with the section name string table
};

struct elf_phdr
{
	u32 p_type;
	u32 p_offset;    // offset from beginning of file to segment
	u32 p_vaddr;     // virtual address at which the first byte of this segment resides in memory
	u32 p_paddr;
	u32 p_filesz;    // number of bytes in the file image of this segment
	u32 p_memsz;     // number of bytes in the memory image of this segment
	u32 p_flags;
	u32 p_align;
};

int execv(const char *pathname, char *const argv[]);

#endif    // ELF_H
