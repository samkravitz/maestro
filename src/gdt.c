#include "gdt.h"

#define NUM_GDT_ENTRIES 5

struct gdt_entry gdt[NUM_GDT_ENTRIES];

static void gdt_set(int n, u32 base, u32 limit, u8 access, u8 granularity);

void gdt_init()
{
    gdt_set(0, 0, 0, 0, 0);                // null segment
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel mode code segment
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel mode data segment
    gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user mode code segment
    gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user mode data segment

    lgdt();
}

static void gdt_set(int n, u32 base, u32 limit, u8 access, u8 granularity)
{
    // set entry's base address
    gdt[n].base_low    = (base >>  0) & 0xFFFF;
    gdt[n].base_middle = (base >> 16) & 0x00FF;
    gdt[n].base_high   = (base >> 24) & 0x00FF;

    // set entry's low limit
    gdt[n].limit_low   = limit & 0xFFFF;

    // set entry's access byte
    gdt[n].present = (access >> 7) & 0x1;
    gdt[n].dpl     = (access >> 5) & 0x3;
    gdt[n].dt      = (access >> 4) & 0x1;
    gdt[n].type    = (access >> 0) & 0xF;

    // set entry's granularity byte
    gdt[n].gran = 1;
    gdt[n].d    = 1;
    gdt[n].zero = 0;
    gdt[n].a    = 0;

    gdt[n].seglen = 0xF;
}

void lgdt()
{
    struct gdtr
    {
        u16 limit;
        u32 base;
    } __attribute__((packed)) gdtr;

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (u32) &gdt;

    __asm("lgdt (%0)" : : "r" (&gdtr));
}