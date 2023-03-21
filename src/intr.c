/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: intr.c
 * DATE: March 24th, 2022
 * DESCRIPTION: High level interrupt service routine
 *		called after bootstrapping in intr.s
 */

#include <intr.h>
#include <io.h>
#include <kprintf.h>
#include <maestro.h>
#include <syscall.h>

#define PIC1 0x20    // pic1 command port
#define PIC2 0xa0    // pic2 command port
#define EOI  0x20    // end of interrupt value

// user registered interrupt handlers
// defined in intr.s
extern void (*user_handlers[])(void);

// system call handlers
// defined in syscall.c
extern void (*syscall_handlers[])(struct registers *);

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
void isr(struct registers *regs)
{
	int mask = disable();
	u8 intr  = regs->intr_num;

	// exception
	if (intr < IRQ0)
	{
		long long cr2;
		asm("movq %%cr2, %0" : "=r"(cr2));

		kprintf("cr2=0x%x\n", cr2);
		kprintf("\n");
		kprintf("\tMAESTRO PANIC!!!\n");
		kprintf("Exception %d: %s\n", intr, xint_msg[intr]);
		kprintf("Error code: %d\n", regs->error_code);
		kprintf("registers: \n");
		kprintf("eax: 0x%x\n", regs->eax);
		kprintf("ebx: 0x%x\n", regs->ebx);
		kprintf("ecx: 0x%x\n", regs->ecx);
		kprintf("edx: 0x%x\n", regs->edx);
		kprintf("esi: 0x%x\n", regs->esi);
		kprintf("edi: 0x%x\n", regs->edi);
		kprintf("ebp: 0x%x\n", regs->ebp);
		kprintf("esp: 0x%x\n", regs->esp);
		kprintf("eip: 0x%x\n", regs->eip);

		while (1)
			;
	}

	// syscall
	else if (intr == SYSCALL)
	{
		u8 sysno = regs->eax;
		if (isbadsysno(sysno))
		{
			kprintf("Bad system call num: %d\n", sysno);
			while (1)
				;
		}

		void (*handler)(struct registers *) = syscall_handlers[sysno];
		handler(regs);
	}

	// irq
	else
	{
		// call registered handler on irq
		void (*handler)(void) = user_handlers[intr];
		handler();

		// acknowledge interrupt with eoi
		outb(PIC1, EOI);

		if (intr > IRQ8)
			outb(PIC2, EOI);
	}

	restore(mask);
}
