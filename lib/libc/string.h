/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/string.h
 * DATE: July 27, 2021
 * DESCRIPTION: memcmp, memcpy, memmove, memset, strcat, strncat, strlen 
 */

#ifndef STRING_H
#define STRING_H

#include <maestro.h>

int indexOf(const char *, char);
int memcmp(const void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
char *strchr(const char *, char);
char *strrchr(const char *, char);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
char *strcpy(char *, const char *);
char *strdup(const char *);
char *strndup(const char *, size_t);
size_t strlen(const char *);
char *strrev(char *);
char *strtok(char *, const char *);

#endif    // STRING_H
