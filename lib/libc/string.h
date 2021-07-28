/* kernel
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/string.h
 * DATE: July 27, 2021
 * DESCRIPTION: memcmp, memcpy, memset, strlen 
 */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int memcmp(const void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
size_t strlen(const char *);

#endif // STRING_H