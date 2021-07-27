/* kernel
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: txtmode.h
 * DATE: July 26, 2021
 * DESCRIPTION: Functions for writing to the VGA text mode monitor
 */

#ifndef TXTMODE_H
#define TXTMODE_H

#include "common.h"

void txtmode_putc(char);
void txtmode_puts(const char *);
void txtmode_clear();

#endif // TXTMODE_H