#ifndef IDT_H
#define IDT_H

#include "common.h"

#define NUM_IDT_ENTRIES 256

struct idt_entry
{
   u16 offlow;    // low offset - bits 0..15 of handler
   u16 selector;  // a code segment selector in GDT or LDT
   u8  zero;      // unused, set to 0
   u8  flags;     // flags (see below)
   u16 offhigh;   // high offset - bits 16..31 of handler
};

void idt_init();
void lidt();
void handler();

/*
 * Explanation of idt_entry flags byte:
 * bit  7: P (present) - should be 1 if the entry is present
 * bits 6-5: DPL (descriptor privilege level) - 0 for kernel mode, 3 for user mode
 * bits 4-0: always 0b01110
 */

#endif // IDT_H