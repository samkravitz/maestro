#include "idt.h"
#include "txtmode.h"

struct idt_entry idt[NUM_IDT_ENTRIES];

void idt_init()
{
    memset(idt, 0, sizeof(idt));
    u32 handler_ptr = (u32) handler;
    u16 offlow  = (handler_ptr >> 0)  & 0xFFFF;
    u16 offhigh = (handler_ptr >> 16) & 0xFFFF;

    // for (int i = 0; i < NUM_IDT_ENTRIES; ++i)
    // {
    //     idt[i].offlow = offlow;
    //     idt[i].selector = 0x8;
    //     idt[i].zero = 0;
    //     idt[i].flags =  0b10001110; //0b10101110;
    //     idt[i].offhigh = offhigh;
    // }

    idt[33].offlow = offlow;
    idt[33].selector = 0x8;
    idt[33].zero = 0;
    idt[33].flags =  0b10001110; //0b10101110;
    idt[33].offhigh = offhigh;

    lidt();
}

void lidt()
{
    struct idtr
    {
        u16 limit;
        u32 base;
    } __attribute__((packed)) idtr;

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (u32) &idt;

    __asm("lidt (%0)" : : "r" (&idtr));

    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    /* Initialization of ICW2 */
    outb(0x21, 0x20);    /* start vector = 32 */
    outb(0xA1, 0x70);    /* start vector = 96 */

    /* Initialization of ICW3 */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    /* Initialization of ICW4 */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /* mask interrupts */
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void handler()
{
    txtmode_puts("Interrupt handled!\n");
}