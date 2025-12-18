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
#include <tty.h>

#include <stdio.h>
#include <stdlib.h>

// Keyboard state
static u32 modifiers = 0;
static bool extended = false;

#define SHIFT_PRESSED() (modifiers & (LSHIFT | RSHIFT))
#define CTRL_PRESSED()  (modifiers & LCTRL)
#define ALT_PRESSED()   (modifiers & LALT)

/**
 * Update modifier key states
 * Returns true if this scancode was a modifier key
 */
static bool update_modifiers(u8 key, bool is_break)
{
	u32 modifier_bit = 0;

	switch (key)
	{
		case LSHIFT_IDX: modifier_bit = LSHIFT; break;
		case RSHIFT_IDX: modifier_bit = RSHIFT; break;
		case LCTRL_IDX: modifier_bit = LCTRL; break;
		case LALT_IDX: modifier_bit = LALT; break;
		case CAPSLOCK_IDX:
			// Toggle on make, ignore on break
			if (!is_break)
				modifiers ^= CAPSLOCK;
			return true;
		default:
			return false;    // Not a modifier
	}

	// Update modifier state
	if (is_break)
		modifiers &= ~modifier_bit;    // Clear
	else
		modifiers |= modifier_bit;    // Set

	return true;
}


// translate a key scancode to a character based on current modifier state
static char translate(u8 key)
{
	if (key >= NUM_KEYS)
		return '\0';

	// Use shifted table if shift is pressed
	if (SHIFT_PRESSED())
		return kbdus_shifted[key];

	char c = kbdus[key];

	// Handle caps lock (only affects letters)
	if ((modifiers & CAPSLOCK) && is_alpha(c))
		c = to_upper(c);

	return c;
}

// handle extended keys (0xE0 prefix)
static void handle_extended(u8 key, bool is_break)
{
	// Only process make codes for now
	if (is_break)
		return;

	// Log extended keys for debugging
	switch (key)
	{
		case EXT_UP: kprintf("[UP]\n"); break;
		case EXT_DOWN: kprintf("[DOWN]\n"); break;
		case EXT_LEFT: kprintf("[LEFT]\n"); break;
		case EXT_RIGHT: kprintf("[RIGHT]\n"); break;
		case EXT_HOME: kprintf("[HOME]\n"); break;
		case EXT_END: kprintf("[END]\n"); break;
		case EXT_PGUP: kprintf("[PGUP]\n"); break;
		case EXT_PGDN: kprintf("[PGDN]\n"); break;
		case EXT_INSERT: kprintf("[INSERT]\n"); break;
		case EXT_DELETE: kprintf("[DELETE]\n"); break;
	}
}

/**
 * Main keyboard interrupt handler
 */
void kbdhandler()
{
	u8 scancode = inb(KBD_IN);

	// Handle multi-byte extended key sequences
	if (scancode == 0xE0)
	{
		extended = true;
		return;
	}

	// Print screen / Pause - ignore for now
	if (scancode == 0xE1)
	{
		return;
	}

	// Separate make (press) from break (release) codes
	bool is_break = (scancode & 0x80) != 0;
	u8 key = scancode & 0x7F;    // Strip the break bit

	// Handle extended keys
	if (extended)
	{
		handle_extended_key(key, is_break);
		extended = false;
		return;
	}

	// Update modifier key states (shift, ctrl, alt, caps)
	if (update_modifiers(key, is_break))
		return;    // Was a modifier, nothing to type

	// Only send characters on make codes (key press, not release)
	if (is_break)
		return;

	// Handle Ctrl+C for interrupt
	if (CTRL_PRESSED() && key == 46)
	{    // 'c' key
		kprintf("^C\n");
		tty_buffer(CTRL('C'));
		return;
	}

	// Handle Ctrl+D for EOF
	if (CTRL_PRESSED() && key == 32)
	{    // 'd' key
		kprintf("^D\n");
		tty_buffer(CTRL('D'));
		return;
	}

	char c = translate(key);

	// only buffer printable characters
	if (c != '\0')
		tty_buffer(c);
}
