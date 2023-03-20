/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kprintf.h
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#ifndef KPRINTF_H
#define KPRINTF_H

#include <maestro.h>
#include <stdarg.h>

#include <string.h>

void kputc(char c);
void kputs(const char *);
int vkprintf(const char *, ...);

/* writes a formatted message to the serial console
 * uses the __FILE__ macro to indicated what file printed the message
 * if this line appeared in the file kmain.c:
 * kprintf("Welcome to %s\n", "maestro!");
 *
 * the serial console would print:
 * [kmain.c]: Welcome to maestro!
 */
#define kprintf(fmt, ...)                           \
{                                                   \
	vkprintf("[%s]: ", strrchr(__FILE__, '/') + 1); \
	vkprintf(fmt, ##__VA_ARGS__);                   \
}

#define kassert(cond, msg)    \
{                             \
    if (!cond)                \
    {                         \
        kprintf("%s\n", msg); \
    }                         \
}                             \

#endif    // KPRINTF_H
