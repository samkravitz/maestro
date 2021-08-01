/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: tty.c
 * DATE: July 26, 2021
 * DESCRIPTION: Functions for writing to the VGA text mode terminalS
 */
#include "tty.h"

#define TTY_WIDTH  80
#define TTY_HEIGHT 25

// attribute byte for white text on black background
#define ATTRIBUTE (0xF << 8)

// get color
#define GETCOL(c) (c | ATTRIBUTE)

// static char framebuffer[TTY_HEIGHT][TTY_WIDTH] = {0};

// coordinate of cursor
static u8 x = 0;
static u8 y = 0;

// vga base address
u16 *VGA_BASE = (u16 *) 0xB8000;

void putc(char c)
{
    // handle special characters
    switch (c)
    {
        case '\n':
            y++;
            x = 0;
            break;

        case '\b':
            if (x > 0)
                x--;
            break;
        
        case '\r':
            x = 0;
            break;
        
        default:
            // is a printable char
            if (c >= 32)
            {
                u16 index = y * TTY_WIDTH + x;
                VGA_BASE[index] = GETCOL(c);
                x++;
            }
    }

    if (x == TTY_WIDTH - 1)
    {
        y++;
        x = 0;
    }
}

// clear - clears the terminal
void clear()
{
    for (int i = 0; i < TTY_WIDTH * TTY_HEIGHT; ++i)
        putc(' ');
}

void puts(const char *str)
{
    while (*str)
        putc(*str++);
}