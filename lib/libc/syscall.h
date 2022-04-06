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

#include <maestro.h>

#define SYS_READ  0
#define SYS_WRITE 1

int syscall(int, ...);

// defined in syscall.s
extern int syscall0(int);
extern int syscall1(int, u32);
extern int syscall2(int, u32, u32);
extern int syscall3(int, u32, u32, u32);

#endif    // SYSCALL_H
