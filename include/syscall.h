/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: syscall.h
 * DATE: April 5th, 2022
 * DESCRIPTION: System call handlers
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <maestro.h>

#include <intr.h>

extern const int NUM_SYSCALLS;

#define isbadsysno(sysno) (sysno >= NUM_SYSCALLS)

void sys_read(struct registers *);
void sys_write(struct registers *);

extern void (*syscall_handlers[])(struct registers *);

#endif	// SYSCALL_H
