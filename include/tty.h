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

#include <termios.h>

void clear();

void tty_init();
int tty_read(void *, size_t);
int tty_write(void *, size_t);
int tty_getc();
void tty_putc(char);
void tty_buffer(int);

#endif    // TTY_H
