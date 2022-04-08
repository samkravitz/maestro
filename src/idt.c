/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: idt.h
 * DATE: August 1st, 2021
 * DESCRIPTION: Initialize the IDT and store it in idtr
 */
#include <idt.h>

#include <intr.h>
#include <io.h>

#include "string.h"

struct idt_entry idt[256];

static void set_idt(int, u32, u16, u8);
static void lidt();

struct idtr
{
	u16 limit;
	u32 base;
} __attribute__((packed)) idtr;

// defined in intr.s
extern void *ivect[];

// init idt
void idt_init()
{
	memset(idt, 0, sizeof(idt));

	// set exception entries in idt
	for (int i = 0; i < 32; ++i)
		set_idt(i, (u32) ivect[i], 0x8, 0x8e);

	// set irq entries in idt
	for (int i = 32; i < 48; ++i)
		set_idt(i, (u32) ivect[i], 0x8, 0x8e);
	
	// set syscall entry in idt
	set_idt(48, (u32) ivect[48], 0x8, 0xee);

	lidt();
}

// stores idt structure in idtr
static void lidt()
{
	idtr.limit = sizeof(idt) - 1;
	idtr.base  = (u32) &idt;

	asm("lidt %0" : : "m"(idtr));
}

// sets a gate in the idt
static void set_idt(int num, u32 base, u16 select, u8 flags)
{
	idt[num].offlow   = (base >> 0) & 0xffff;
	idt[num].offhigh  = (base >> 16) & 0xffff;
	idt[num].selector = select;
	idt[num].type     = (flags >> 0) & 0xf;
	idt[num].sseg     = (flags >> 4) & 0x1;
	idt[num].dpl      = (flags >> 5) & 0x3;
	idt[num].present  = (flags >> 7) & 0x1;
	idt[num].zero     = 0;
}
