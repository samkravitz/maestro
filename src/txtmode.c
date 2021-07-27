/* kernel
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: txtmode.h
 * DATE: July 26, 2021
 * DESCRIPTION: Functions for writing to the VGA text mode monitor
 */

#include "txtmode.h"

#define TXTMODE_WIDTH  80
#define TXTMODE_HEIGHT 25

// attribute byte for white text on black background
#define ATTRIBUTE (0xF << 8)

// get color
#define GETCOL(c) (c | ATTRIBUTE)

// static char framebuffer[TXTMODE_HEIGHT][TXTMODE_WIDTH] = {0};

// coordinate of cursor
static u8 x = 0;
static u8 y = 0;

// vga base address
u16 *VGA_BASE = (u16 *) 0xB8000;

void txtmode_putc(char c)
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
                u16 index = y * TXTMODE_WIDTH + x;
                VGA_BASE[index] = GETCOL(c);
                x++;
            }
    }

    if (x == TXTMODE_WIDTH - 1)
    {
        y++;
        x = 0;
    }
}

void txtmode_clear()
{
    for (int y = 0; y < TXTMODE_HEIGHT; ++y)
        for (int x = 0; x < TXTMODE_WIDTH; ++x)
            txtmode_putc(' ');
}

void txtmode_puts(const char *str)
{
    while (*str)
        txtmode_putc(*str++);
}