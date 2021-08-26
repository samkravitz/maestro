/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kout.h
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#ifndef kout_H
#define kout_H

#include <maestro.h>
#include <stdarg.h>

void kout(const char *);
int koutf(const char *, ...);

#endif // kout_H