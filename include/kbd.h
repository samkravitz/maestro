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

#include <maestro.h>

#define NUM_KEYS     128
#define KBD_IN       0x60

// indices of special keys in LUT
#define ESC_IDX      1
#define LCTRL_IDX    28
#define LSHIFT_IDX   42
#define RSHIFT_IDX   54
#define LALT_IDX     56
#define CAPSLOCK_IDX 68

// bitshift values to test keyboard state quickly
#define ESC          0x1
#define LCTRL        0x2
#define LSHIFT       0x4
#define RSHIFT       0x8
#define LALT         0x10
#define CAPSLOCK     0x20

// extended key codes (scancodes after 0xE0 prefix)
#define EXT_UP       0x48
#define EXT_DOWN     0x50
#define EXT_LEFT     0x4B
#define EXT_RIGHT    0x4D
#define EXT_HOME     0x47
#define EXT_END      0x4F
#define EXT_PGUP     0x49
#define EXT_PGDN     0x51
#define EXT_INSERT   0x52
#define EXT_DELETE   0x53

// Control character helper
#define CTRL(c)      ((c) & 0x1F)

/*
 * scancode -> key look up table
 * scancode set 1 - US QWERTY
 * if an entry in this table is '\0', it corresponds to a special key (shifts, capslock, etc)
 * in that case, the comments to the right of the line explain what the special key is
 */
static const char kbdus[NUM_KEYS] = {
	'\0', '\0', '1',  '2',  '3',  '4',  '5',  '6',    // NONE, ESC
	'7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
	'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
	'o',  'p',  '[',  ']',  '\n', '\0', 'a',  's',    // LCTRL
	'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
	'\'', '`',  '\0', '\\', 'z',  'x',  'c',  'v',    // LSHIFT
	'b',  'n',  'm',  ',',  '.',  '/',  '\0', '*',    // RSHIFT
	'\0', ' ',  '\0', '1',  '2',  '3',  '4',  '5',    // LALT, CAPSLOCK, F1, F2, F3, F4, F5
	'6',  '7',  '8',  '9',  '0',  '\0', '\0', '7',    // F6, F7, F8, F9, F10, NUMLOCK, SCROLLOCK
	'8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',    // KEYPAD NUMS
	'2',  '3',  '0',  '.',  '\0', '\0', '\0', '\0',   // NONE, NONE, NONE, F11
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',   // F12, THEN ALL NONE
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
};

/*
 * shifted scancode -> key look up table
 * scancode set 1 - US QWERTY with shift modifier
 */
static const char kbdus_shifted[NUM_KEYS] = {
	'\0', '\0', '!',  '@',  '#',  '$',  '%',  '^',    // NONE, ESC
	'&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
	'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
	'O',  'P',  '{',  '}',  '\n', '\0', 'A',  'S',    // LCTRL
	'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
	'"',  '~',  '\0', '|',  'Z',  'X',  'C',  'V',    // LSHIFT
	'B',  'N',  'M',  '<',  '>',  '?',  '\0', '*',    // RSHIFT
	'\0', ' ',  '\0', '!',  '@',  '#',  '$',  '%',    // LALT, CAPSLOCK, F1, F2, F3, F4, F5
	'^',  '&',  '*',  '(',  ')',  '\0', '\0', '7',    // F6, F7, F8, F9, F10, NUMLOCK, SCROLLOCK
	'8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',    // KEYPAD NUMS
	'2',  '3',  '0',  '.',  '\0', '\0', '\0', '\0',   // NONE, NONE, NONE, F11
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',   // F12, THEN ALL NONE
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
};

void kbdhandler();

#endif    // KBD_H
