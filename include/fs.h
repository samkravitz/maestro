/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: fs.h
 * DATE: August 30, 2021
 * DESCRIPTION: virtual filesystem abstraction
 */
#ifndef FS_H
#define FS_H

#include "stdio.h"

int close(int);
FILE *open(const char *);
size_t read(int, void *, size_t);
size_t write(int, const void*, size_t);

void fsinit();
void mount();

#endif // FS_H