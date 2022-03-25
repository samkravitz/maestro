/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: isr.c
 * DATE: March 24th, 2022
 * DESCRIPTION: High level interrupt service routine
 *		called after bootstrapping in intr.s
 */

#include <intr.h>
#include <io.h>
#include <kprintf.h>
#include <maestro.h>

#define PIC1 0x20    // pic1 command port
#define PIC2 0xa0    // pic2 command port
#define EOI  0x20    // end of interrupt value

// state of the stack when an interrupt is called
// see isr_common in intr.s
struct state
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

// user registered interrupt handlers
// defined in intr.s
extern void (*user_handlers[])(void);

// exception messages
static const char *xint_msg[] = {
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
	"reserved",
	"floating point error",
	"alignment check",
	"machine check",
	"floating point exception",
	"virtualization exception",
	"control protection exception",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"hypervisor injection exception",
	"vmm communication exception",
	"security exception",
	"reserved",
};

/**
 * @brief high level interrupt handler
 * common assembly code in intr.s bootstraps the handler
 * and calls this function. This function is not meant to be
 * called any other way. Its parameter is a pointer to the top
 * of the stack after the assembly bootstrap saves its state
 */
void isr(void *p)
{
	struct state *state = (struct state *) p;
	u8 intr = state->intr_num;

	// panic on exception
	if (intr < IRQ0)
	{
		kprintf("\n");
		kprintf("\tMAESTRO PANIC!!!\n");
		kprintf("Exception %d: %s\n", intr, xint_msg[intr]);
		kprintf("Error code: %d\n", state->error_code);
		kprintf("registers: \n");
		kprintf("eax: 0x%x\n", state->eax);
		kprintf("ebx: 0x%x\n", state->ebx);
		kprintf("ecx: 0x%x\n", state->ecx);
		kprintf("edx: 0x%x\n", state->edx);
		kprintf("esi: 0x%x\n", state->esi);
		kprintf("edi: 0x%x\n", state->edi);
		kprintf("ebp: 0x%x\n", state->ebp);
		kprintf("esp: 0x%x\n", state->esp);
		kprintf("eip: 0x%x\n", state->eip);

		while (1)
			;
	}

	// call registered handler on irq
	void (*handler)(void) = user_handlers[intr];
	handler();

	// acknowledge interrupt with eoi
	outb(PIC1, EOI);

	if (intr > IRQ8)
		outb(PIC2, EOI);
}
