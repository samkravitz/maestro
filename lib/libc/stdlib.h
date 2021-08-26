#ifndef STDLIB_H
#define STDLIB_H

#include <maestro.h>

#define is_alpha(c)     ((c >= 'a') && (c <= 'z'))
#define is_numeric(c)	((c >= '0') && (c <= '9'))
#define to_upper(c)     ('A' + c - 'a')

int atoi(const char *);
char *itoa(u32, char *, int, int);

#endif // STDLIB_H