/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: tty.h
 * DATE: July 26, 2021
 * DESCRIPTION: Functions for writing to the VGA text mode terminal
 */
#ifndef TTY_H
#define TTY_H

#include <maestro.h>

void putc(char);
void puts(const char *);
void clear();

int tty_getc();
void tty_putc(int c);

#endif    // TTY_H
