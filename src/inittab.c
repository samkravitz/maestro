#include "inittab.h"

#include "gdt.h"
#include "idt.h"

// inittab - initialize GDT and IDT tables
void inittab()
{
    gdt_init();
    //idt_init();
}