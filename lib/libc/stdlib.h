#ifndef STDLIB_H
#define STDLIB_H

#include <maestro.h>

#define is_numeric(c)	((c >= '0') && (c <= '9'))
int atoi(const char *);
char *itoa(u32, char *, int);

#endif // STDLIB_H