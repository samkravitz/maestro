/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kprintf.h
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#ifndef KPRINTF_H
#define KPRINTF_H

#include <maestro.h>
#include <stdarg.h>

void kputc(char c);
void kputs(const char *);
int kprintf(const char *, ...);

#endif    // KPRINTF_H
