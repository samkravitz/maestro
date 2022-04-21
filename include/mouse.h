/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: mouse.h
 * DATE: April 20th, 2022
 * DESCRIPTION: PS/2 mouse driver
 */
#ifndef MOUSE_H
#define MOUSE_H

#include <maestro.h>

#define MOUSE_DATA   0x60    // mouse input port
#define MOUSE_STATUS 0x64    // mouse status port
#define MOUSE_CMD    0xd4    // value to send mouse before command

#define WAIT_IN      1
#define WAIT_OUT     2

void mouse_init();

#endif    // MOUSE_H
