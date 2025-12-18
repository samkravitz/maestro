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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
	char name[32];
	size_t len;
	uint32_t eof;
	uint32_t position;
	int fd;
} FILE;

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define EOF          -1

#define SEEK_SET 0

int fflush(FILE *);
FILE *fopen(const char *, const char *);
int fprintf(FILE *, const char *, ...);
size_t fread(void *, size_t, size_t, FILE *);
int fseek(FILE *, long, int);
long ftell(FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int printf(const char *, ...);
void setbuf(FILE *, char *);
int sprintf(char *, const char *, ...);
int vprintf(const char *, va_list);
int vfprintf(FILE *, const char *, va_list);
int vsprintf(char *, const char *, va_list);

int getc();

#endif    // STDIO_H
