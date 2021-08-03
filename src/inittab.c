/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: init.c
 * DATE: August 1st, 2021
 * DESCRIPTION: Initializes tables vital for kernel
 */
#include "init.h"

#include "idt.h"
#include "intr.h"
#include "pit.h"

// inittab - initializes IDT table
void inittab()
{
    intr_init();
    idt_init();
    timer_init();
    asm("sti"); // enable interrupts
}
