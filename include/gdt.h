#ifndef GDT_H
#define GDT_H

#include "common.h"

struct gdt_entry
{
    u16 limit_low;
    u16 base_low;
    u8  base_middle;
    u8  present : 1;
    u8  dpl     : 2;
    u8  dt      : 1;
    u8  type    : 4;
    u8  gran    : 1;
    u8  d       : 1;
    u8  zero    : 1;
    u8  a       : 1;
    u8  seglen  : 4;
    u8  base_high;
} __attribute__((packed));

void gdt_init();
void lgdt();
//static void gdt_set(int, u32, u32, u8, u8);

#endif // GDT_H