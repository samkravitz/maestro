/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: inittab.c
 * DATE: August 1st, 2021
 * DESCRIPTION: Initializes tables vital for kernel
 */
#include "inittab.h"

#include "idt.h"
#include "intr.h"

// inittab - initializes IDT table
void inittab()
{
    intr_init();
    idt_init();
}