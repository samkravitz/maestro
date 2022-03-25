/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: idt.h
 * DATE: August 1st, 2021
 * DESCRIPTION: Declarations for IDT structures and functions
 */
#ifndef IDT_H
#define IDT_H

#include <maestro.h>

void idt_init();

struct idt_entry
{
	u16 offlow;        // low offset - bits 0..15 of handler
	u16 selector;      // a code segment selector in GDT or LDT
	u8 zero;           // unused, set to 0
	u8 type    : 4;    // gate type
	u8 sseg    : 1;    // storage segment - set to 0 for interrupt and trap gates
	u8 dpl     : 2;    // descriptor privilege level
	u8 present : 1;    // set to 0 for unused interrupts
	u16 offhigh;       // high offset - bits 16..31 of handler
} __attribute__((packed));

#endif    // IDT_H
