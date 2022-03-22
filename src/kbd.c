/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kbd.c
 * DATE: August 2nd, 2021
 * DESCRIPTION: Driver for PS/2 keyboard
 */
#include <kbd.h>

#include <io.h>

#include "stdio.h"
#include "stdlib.h"

// state of special characters
static u32 state = 0;

#define PRESSED(k) (state & (k))

void kbdhandler()
{
	u8 scancode = inb(KBD_IN);

	if (scancode >= NUM_KEYS)
		return;

	char c = kbdus[scancode];

	// check for special keys that could alter key state, like shifts
	switch (scancode)
	{
		// left shift pressed
		case LSHIFT_IDX:        state |= LSHIFT;  break;

		// left shift released
		case LSHIFT_IDX + 0x80: state &= ~LSHIFT; break;

		// right shift pressed
		case RSHIFT_IDX:        state |= RSHIFT;  break;

		// right shift released
		case RSHIFT_IDX + 0x80: state &= ~RSHIFT; break;
	}

	if (is_alpha(c) && PRESSED(LSHIFT | RSHIFT))
		c = to_upper(c);

	// TODO - this should be buffered so it can be read from stdin
	printf("%c", c);
}
