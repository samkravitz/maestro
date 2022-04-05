/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: intr.h
 * DATE: August 2nd, 2021
 * DESCRIPTION: declarations for common interrupt related functions
 */
#ifndef INTR_H
#define INTR_H

#include <maestro.h>

#define IRQ0           32    // Programmable Interrupt Timer Interrupt
#define IRQ1           33    // Keyboard Interrupt
#define IRQ2           34    // Cascade (used internally by the two PICs. never raised)
#define IRQ3           35    // COM2 (if enabled)
#define IRQ4           36    // COM1 (if enabled)
#define IRQ5           37    // LPT2 (if enabled)
#define IRQ6           38    // Floppy Disk
#define IRQ7           39    // LPT1 / Unreliable "spurious" interrupt (usually)
#define IRQ8           40    // CMOS real-time clock (if enabled)
#define IRQ9           41    // Free for peripherals / legacy SCSI / NIC
#define IRQ10          42    // Free for peripherals / SCSI / NIC
#define IRQ11          43    // Free for peripherals / SCSI / NIC
#define IRQ12          44    // PS2 Mouse
#define IRQ13          45    // FPU / Coprocessor / Inter-processor
#define IRQ14          46    // Primary ATA Hard Disk
#define IRQ15          47    // Secondary ATA Hard Disk

#define SYSCALL        48    // system call interrupt number

// state of the registers pushed on the stack when an interrupt occurs 
// see isr_common in intr.s
struct registers
{
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 esp;    // esp at time of interrupt
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 intr_num;
	u32 error_code;
	u32 eip;
};

// defined in intr.s
void intr_init();
extern void set_vect(u8, void (*)(void));
extern int disable();
extern void restore(int);

// defined in isr.c
void isr(struct registers *);

#endif    // INTR_H
