/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/unistd.h
 * DATE: April 6th, 2022
 * DESCRIPTION: POSIX compatibility definitions
 */

#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>
#include "sys/types.h"

size_t read(int, void *, size_t);
size_t write(int, void *, size_t);

int execv(const char*, char* const[]);
int execve(const char*, char* const[], char* const[]);
int execvp(const char*, char* const[]);
pid_t fork(void);
void *sbrk(intptr_t);

#endif    // UNISTD_H
