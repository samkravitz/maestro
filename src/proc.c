#include <proc.h>

#include <dump.h>
#include <intr.h>
#include <pq.h>

#include "malloc.h"
#include "string.h"

struct proc *curr;
struct stackframe *currframe;
struct proc *proctab[NPROC];
struct pq *readylist = NULL;

extern u32 *prAstkptr;
extern u32 *prBstkptr;
extern u32 *nullstkptr;

struct proc *create(void (*func)(void))
{
	struct proc *prptr = (struct proc *) malloc(sizeof(struct proc));
	// memcpy(prptr->name, name, strlen(name) + 1);
	// prptr->name[strlen(name)] = '\0';
	struct stackframe *frame = (struct stackframe *) (prptr->stack + 4096 - sizeof(struct stackframe));
	frame->eax = 0;
	frame->ebx = 0;
	frame->ecx = 0;
	frame->edx = 0;
	frame->esi = 0;
	frame->edi = 0;
	frame->ebp = 0;
	// frame->gs = 0;
	// frame->fs = 0;
	// frame->es = 0;
	// frame->ds = 0;

	frame->eip = (uptr) func;
	frame->cs = 0x8;
	frame->eflags = 0x202;

	// frame->flags = 0x202;
	// frame->cs    = 8;
	// frame->eip   = (uptr) func;
	// frame->ebp   = 0;
	// frame->esp   = 0;
	// frame->edi   = 0;
	// frame->esi   = 0;
	// frame->edx   = 0;
	// frame->ecx   = 0;
	// frame->ebx   = 0;
	// frame->eax   = 0;
	// frame->ds    = 0x10;
	// frame->es    = 0x10;
	// frame->fs    = 0x10;
	// frame->gs    = 0x10;
	// prptr->esp = (uptr) frame;
	// prptr->ss = 0x10;
	prptr->frame = frame;
	//stackdump((u32 *) frame, 10);
	return prptr;
}

// struct proc *prspawn(void (*func)(void), char *name, int a)
// {
// 	disable();
// 	static int pid = 0;

// 	struct proc *prptr = (struct proc *) malloc(sizeof(struct proc));
// 	u32 *stkptr;
// 	if (a == 1) {
// 		stkptr = prAstkptr;
// 		kout ("malloc");
// 	}
// 	else if (a == 0) {
// 		stkptr = prBstkptr;
// 		kout ("free");
// 	}
// 	else {
// 		stkptr = nullstkptr;
// 	}
// 	prptr->pid = pid++;
// 	prptr->stkbase = stkptr;
// 	prptr->stkptr = stkptr;
// 	memcpy(prptr->name, name, strlen(name) + 1);
// 	prptr->name[strlen(name)] = '\0';

// 	#define	STACKMAGIC	0x0A0AAAA9

// 	koutf("func is %x\n", func);

// 	u32 savsp, *pushsp;
// 	u32 *saddr = (u32) stkptr;
// //	*--saddr = func;

// 	*saddr = STACKMAGIC;
// 	savsp = (u32)saddr;

// 	/* Push arguments */
// 	*--saddr = (u32) prret;	/* Push on return address	*/

// 	/* The following entries on the stack must match what ctxsw	*/
// 	/*   expects a saved process state to contain: ret address,	*/
// 	/*   ebp, interrupt mask, flags, registers, and an old SP	*/
// 	//*--saddr = (long)func;

// 	//*--saddr = (long)func;
// 	*--saddr = (long)func;	/* Make the stack look like it's*/
// 					/*   half-way through a call to	*/
// 					/*   ctxsw that "returns" to the*/
// 					/*   new process		*/
// 	*--saddr = savsp;		/* This will be register ebp	*/
// 					/*   for process exit		*/
// 	savsp = (u32) saddr;		/* Start of frame for ctxsw	*/
// 	*--saddr = 0x00000200;		/* New process runs with	*/
// 					/*   interrupts enabled		*/

// 	/* Basically, the following emulates an x86 "pushal" instruction*/

// 	*--saddr = 0;			/* %eax */
// 	*--saddr = 0;			/* %ecx */
// 	*--saddr = 0;			/* %edx */
// 	*--saddr = 0;			/* %ebx */
// 	*--saddr = 0;			/* %esp; value filled in below	*/
// 	pushsp = saddr;			/* Remember this location	*/
// 	*--saddr = savsp;		/* %ebp (while finishing ctxsw)	*/
// 	*--saddr = 0;			/* %esi */
// 	*--saddr = 0;			/* %edi */
// 	*pushsp = (unsigned long) (prptr->stkptr = (char *)saddr);
// 	//restore(mask);
// 	enable();
// 	return prptr;

// 	// create dummy stack frame for process
// 	// u32 *sp = (u32 *) stkptr;
// 	// *sp = (u32) prret; sp--;
// 	// *sp = (u32) func; sp--;


// 	// *sp = 0; sp--; 			// eax
// 	// *sp = 0; sp--; 			// ebx
// 	// *sp = 0; sp--; 			// edx
// 	// *sp = 0; sp--; 			// ebx
// 	// *sp = (u32) stkptr; sp--;	// temp - stack ptr before push began
// 	// *sp = (u32) stkptr; sp--;	// ebp
// 	// *sp = 0; sp--;			// esi
// 	// *sp = 0; sp--;			// edi

// 	// u32 eflags = 0x200;		// interrupts enabled
// 	// *sp = eflags; sp--;
// 	// prptr->stkptr = sp;
// 	// koutf("stkptr is: %x and new is: %x\n", stkptr, prptr->stkptr);
// 	// enable();
// 	// return prptr;
// }

void prret()
{
	koutf("Process has returned!\n");
}
