/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/stdio.h
 * DATE: March 22nd, 2022
 * DESCRIPTION: standard input/output utilities 
 */
#ifndef STDIO_H
#define STDIO_H

#include <maestro.h>
#include <stdarg.h>

typedef struct
{
	char name[32];
	size_t len;
	u32 eof;
	u32 position;
	int fd;
} FILE;

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

int printf(const char *, ...);
int fprintf(FILE *, const char *, ...);
int sprintf(char *, const char *, ...);

int vprintf(const char *, va_list);
int vfprintf(FILE *, const char *, va_list);
int vsprintf(char *, const char *, va_list);

int getc();

#endif    // STDIO_H
