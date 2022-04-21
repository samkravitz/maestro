/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: w.h
 * DATE: April 20th, 2022
 * DESCRIPTION: w windowing and graphics system
 */
#ifndef W_H
#define W_H

#include <maestro.h>

struct gfx_info
{
	u16 width;          // screen width in pixels
	u16 height;         // screen height in pixels
	u16 pitch;          // number of bytes in 1 line
	u8 bpp;             // bits per pixel
	u32 framebuffer;    // physical address of linear framebuffer
} __attribute__((packed));

void w_init();

#endif    // W_H
