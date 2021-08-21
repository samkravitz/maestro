#include <proc.h>

#include <intr.h>
#include <pq.h>

#include "malloc.h"
#include "string.h"

struct proc *curr;
struct proc *proctab[NPROC];

// where processes return when they terminate
static void pterm() { }

struct proc *create(void (*func)(void), const char *name)
{
	struct proc *prptr = (struct proc *) malloc(sizeof(struct proc));
	memcpy(prptr->name, name, strlen(name) + 1);
	prptr->name[strlen(name)] = '\0'; 

	// struct stackframe *frame = (struct stackframe *) (prptr->stack + 1023 - sizeof(struct stackframe));
	// u32 tmp = frame;
	// frame->edi = 0;
	// frame->esi = 0;
	// frame->ebp = 0;
	// frame->esp = 0;
	// frame->ebx = 0;
	// frame->edx = 0;
	// frame->ecx = 0;
	// frame->eax = 0x420;
	// frame->eflags = 0x202;
	// frame->eip = func;

	// prptr->frame = frame;


	u32 *stkptr = prptr->stack + 1024;

	u32 tmp = stkptr;

	stkptr--; *stkptr = pterm;
	stkptr--; *stkptr = func;
	stkptr--; *stkptr = tmp;
	tmp = stkptr;

	//stkptr--; *stkptr = func;
	stkptr--; *stkptr = 0x202;
	stkptr--; *stkptr = 0;		// eax
	stkptr--; *stkptr = 0;		// ecx
	stkptr--; *stkptr = 0;		// edx
	stkptr--; *stkptr = 0;		// ebx
	stkptr--; *stkptr = 0;		// esp
	stkptr--; *stkptr = tmp;	// ebp
	stkptr--; *stkptr = 0;		// esi
	stkptr--; *stkptr = 0;		// edi

	prptr->stkptr = stkptr;
	return prptr;
}
