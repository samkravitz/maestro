/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: klog.c
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#include "klog.h"

#include "txtmode.h"

void klog(const char *msg)
{
	txtmode_puts(msg);
}

void klogh(u32 x)
{
	klog("0x");

	char c, hex;
	while (x)
	{
		c = x % 16;
		
		if (c < 10)
			hex = c + '0';
		
		else
			hex = (c - 10) + 'A';

		txtmode_putc(hex);
		x /= 16;
	}
}