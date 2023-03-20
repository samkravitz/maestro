/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/syscall.h
 * DATE: April 5th, 2022
 * DESCRIPTION: C library syscall wrappers
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_READ     0
#define SYS_WRITE    1
#define SYS_EXIT     2
#define SYS_OPEN     3
#define SYS_SBRK     4
#define SYS_GETDENTS 5
#define SYS_FORK     6

int syscall(int, ...);

// defined in syscall.s
extern int syscall0(int);
extern int syscall1(int, uint32_t);
extern int syscall2(int, uint32_t, uint32_t);
extern int syscall3(int, uint32_t, uint32_t, uint32_t);

#endif    // SYSCALL_H
