/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: io.h
 * DATE: July 28, 2021
 * DESCRIPTION: outb, inb, inw
 */
#ifndef IO_H
#define IO_H

#include "common.h"

void outb(u16, u8);
u8 inb(u16);
u16 inw(u16);

#endif // IO_H