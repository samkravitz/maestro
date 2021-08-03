/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kbd.h
 * DATE: August 2nd, 2021
 * DESCRIPTION: Driver for PS/2 keyboard
 */
#ifndef KBD_H
#define KBD_H

void kbd_init();

// scancode -> key look up table
// scan code set 1
static const char keylut[128] = {
    '\0', '\0', '1', '2', '3', '4', '5', '6',		// NONE, ESC
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', '\0', 'a', 's',       // LCTRL
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',      // LSHIFT
    'b', 'n', 'm', ',', '.', '/', '\0','*',         // RSHIFT
    '\0', ' ', '\0', '1', '2', '3', '4', '5',       // LALT, CAPSLOCK, F1, F2, F3, F4, F5
    '6', '7', '8', '9', '0', '\0', '\0', '7',		// F6, F7, F8, F9, F10, NUMLOCK, SCROLLOCK
    '8', '9', '-', '4', '5', '6', '+', '1',			// KEYPAD NUMS
    '2', '3', '0', '.', '\0', '\0', '\0', '\0',     // NONE, NONE, NONE, F11
	'\0',											// F12
};


#endif // KBD_H