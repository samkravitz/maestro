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

#include "common.h"

void iidt();

struct idt_entry
{
   u16 offlow;      // low offset - bits 0..15 of handler
   u16 selector;    // a code segment selector in GDT or LDT
   u8  zero;        // unused, set to 0
   u8  type    : 4; // gate type
   u8  sseg    : 1; // storage segment - set to 0 for interrupt and trap gates
   u8  dpl     : 2; // descriptor privilege level
   u8  present : 1; // set to 0 for unused interrupts
   u16 offhigh;     // high offset - bits 16..31 of handler
} __attribute__((packed));

// defined in isr.s
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// defined in irq.s
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

// interrupt table
// these are not the interrupt handlers themselves,
// but save program state and jump to actual registered handler
static void (* const isrtab[]) (void) = {
	isr0,   isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,
	isr8,   isr9, isr10, isr11, isr12, isr13, isr14, isr15,
	isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
	isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31,
	irq0,   irq1,  irq2,  irq3,  irq4,  irq5,  irq6,  irq7,
	irq8,  irq9,  irq10, irq11, irq12, irq13, irq14, irq15
};

#endif // IDT_H