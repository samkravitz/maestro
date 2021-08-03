/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: intr.c
 * DATE: August 2nd, 2021
 * DESCRIPTION: Combined dispatchers for isr and irq
 */
#include "intr.h"

#include "io.h"
#include "klog.h"

// holds registered interrupt handlers
void (*intr_handlers[NUM_INTERRUPTS]) (void) = {0};

// dummy nop function
static void nop() { asm("nop"); }

// end of interrupt - acknowledges interrupt to PIC
// @param x - index of acknowledged interrupt
void eoi(int x)
{
    // clear PIC2
    if (x >= 40)
        outb(0xA1, 0x20);
    
    // clear PIC1
	outb(0x20, 0x20);
}

void intr_init()
{
	// fill with dummy nop function so unregistered handlers are safe
	for (int i = 0; i < NUM_INTERRUPTS; i++)
        intr_handlers[i] = nop;
}

// dispatches the registered irq
// @param x - index of registered interrupt (0-255)
void irq(int x)
{
    if (x < 0 || x >= 255)
        return;
    
    eoi(x);

    void (*handler)(void) = intr_handlers[x];
    handler();
}

void isr()
{
	klog("isr handler!\n");
}

void register_interrupt_handler(int x, void (*handler)(void))
{
	if (x < 0 || x >= 255)
        return;

    intr_handlers[x] = handler;
}