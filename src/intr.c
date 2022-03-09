/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: intr.c
 * DATE: August 2nd, 2021
 * DESCRIPTION: Combined dispatchers for isr and irq
 */
#include <intr.h>

#include <io.h>
#include <kprintf.h>

// holds registered interrupt handlers
void (*intr_handlers[NUM_INTERRUPTS])(void) = { 0 };

// dummy nop function
static void nop()
{
	asm("nop");
}

// end of interrupt - acknowledges interrupt to PIC
// @param x index of acknowledged interrupt
void eoi(int x)
{
	// clear PIC2
	if (x >= 40)
		outb(0xA1, 0x20);

	// clear PIC1
	outb(0x20, 0x20);
}

// init interrupts
void intrinit()
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

void isr(struct state s)
{
	panic(s);
}

void panic(struct state s)
{
	kprintf("\nMAESTRO PANIC!!!\n");

	if (s.inum < NMSG)
		kprintf("Exception %d: %s\n", s.inum, excmsg[s.inum]);

	kprintf("registers: \n");
	kprintf("eax: %x\n", s.eax);
	kprintf("ebx: %x\n", s.ebx);
	kprintf("ecx: %x\n", s.ecx);
	kprintf("edx: %x\n", s.edx);
	kprintf("esi: %x\n", s.esi);
	kprintf("edi: %x\n", s.edi);
	kprintf("ebp: %x\n", s.ebp);
	kprintf("esp: %x\n", s.esp);

	if (s.errcode)
		kprintf("Error code: %d\n", s.errcode);

	kprintf("Panic complete...\n");

	while (1)
		;
}

/* set exception vector handler
 * @param x index of interrupt to register (0-255)
 * @param handler the handler function
 */
void svect(int x, void (*handler)(void))
{
	if (x < 0 || x >= 255)
		return;

	intr_handlers[x] = handler;
}