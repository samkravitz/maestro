#ifndef STATE_H
#define STATE_H

#include <maestro.h>

struct state
{
	u32 edi;    // pushed by pusha
	u32 esi;
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 inum;       // interrupt number
	u32 errcode;    // error code (if applicable)
	u32 eip;        // pushed by processor during exception
	u32 cs;
	u32 eflags;
	u32 useresp;
	u32 ss;
};

#endif    // STATE_H