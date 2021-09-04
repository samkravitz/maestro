/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/stdio.h
 * DATE: August 30, 2021
 * DESCRIPTION: standard input/output utilities 
 */
#ifndef STDIO_H
#define STDIO_H

#include <maestro.h>

// ugh, I hate typedef...
typedef struct
{
	char name[32];
	size_t len;
	u32 eof;
	u32 position;
	int fd;
} FILE;

#endif // STDIO_H