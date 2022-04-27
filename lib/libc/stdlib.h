#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

#define is_alpha(c)   ((c >= 'a') && (c <= 'z'))
#define is_numeric(c) ((c >= '0') && (c <= '9'))
#define to_upper(c)   ('A' + c - 'a')

int atoi(const char *);
void pad(char *, int, char);
char *itoa(uint32_t, char *, int);

void abort(void);
int atexit(void (*)(void));
void *calloc(size_t, size_t);
void free(void *);
char *getenv(const char *);
void *malloc(size_t);
void *realloc(void *, size_t);

#endif    // STDLIB_H
