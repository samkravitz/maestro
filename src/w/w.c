/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: w.c
 * DATE: April 20th, 2022
 * DESCRIPTION: w windowing and graphics system
 */
#include <w.h>

#include <kprintf.h>

struct color
{
	u8 b;
	u8 g;
	u8 r;
} __attribute__((packed));

static int width, height, bpp, pitch;
static struct color *fb;

void w_init()
{
	// bootloader places graphics info at address 0x1000
	struct gfx_info *info = (struct gfx_info *) 0x1000;
	kprintf("height: %d\n", info->height);
	kprintf("width: %d\n", info->width);
	kprintf("bpp: %d\n", info->bpp);
	kprintf("pitch: %d\n", info->pitch);
	kprintf("framebuffer: %x\n", info->framebuffer);

	width  = info->width;
	height = info->height;
	bpp    = info->bpp;
	pitch  = info->pitch;
	fb     = (struct color *) info->framebuffer;

	// paint the screen blue
	for (int i = 0; i < width * height; i++)
	{
		fb[i].r = 0;
		fb[i].g = 0;
		fb[i].b = 0xff;
	}
}