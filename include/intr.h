/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: intr.h
 * DATE: August 2nd, 2021
 * DESCRIPTION: Combined dispatchers for isr and irq
 */
#ifndef INTR_H
#define INTR_H

#include <state.h>
#include <kout.h>

#define NUM_INTERRUPTS 		256

#define IRQ0	 32			// Programmable Interrupt Timer Interrupt
#define IRQ1	 33			// Keyboard Interrupt
#define IRQ2	 34			// Cascade (used internally by the two PICs. never raised)
#define IRQ3	 35			// COM2 (if enabled)
#define IRQ4	 36			// COM1 (if enabled)
#define IRQ5	 37			// LPT2 (if enabled)
#define IRQ6	 38			// Floppy Disk
#define IRQ7	 39			// LPT1 / Unreliable "spurious" interrupt (usually)
#define IRQ8	 40			// CMOS real-time clock (if enabled)
#define IRQ9	 41			// Free for peripherals / legacy SCSI / NIC
#define IRQ10	 42			// Free for peripherals / SCSI / NIC
#define IRQ11	 43			// Free for peripherals / SCSI / NIC
#define IRQ12	 44			// PS2 Mouse
#define IRQ13	 45			// FPU / Coprocessor / Inter-processor
#define IRQ14	 46			// Primary ATA Hard Disk
#define IRQ15	 47			// Secondary ATA Hard Disk

// holds registered interrupt handlers
extern void (*intr_handlers[NUM_INTERRUPTS]) (void);

void eoi(int);
void intrinit();
void irq(int);
void isr(struct state);
void panic(struct state);

void svect(int, void (*)(void));

// disable / enable interrupts
extern int disable();
extern void restore(int);

// exception messages
static const char *excmsg[] = {
    "divide error",
    "debug exception",
    "nmi interrupt",
    "breakpoint",
    "overflow",
    "bound range exceeded",
    "invalid opcode",
    "device not available",
    "double fault",
    "coprocessor segment overrun",
    "invalid tss",
    "segment not present",
    "stack segment fault",
    "general protection",
    "page fault",
    "floating point error",
    "alignment check",
    "machine check",
    "floating point exception",
    "virtualization exception",
};

// num exception messages
static const u32 NMSG = sizeof(excmsg) / sizeof(char *);

#endif // INTR_H
