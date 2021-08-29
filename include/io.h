/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: io.h
 * DATE: July 28, 2021
 * DESCRIPTION: read/write to IO ports
 */
#ifndef IO_H
#define IO_H

#include <maestro.h>

void outb(u16, u8);
void outw(u16, u16);
u8 inb(u16);
u16 inw(u16);

#endif // IO_H