/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: ext2.h
 * DATE: September 8, 2021
 * DESCRIPTION: ext2 filesystem driver
 */
#ifndef EXT2_H
#define EXT2_H

#include <maestro.h>

#define EXT2_OFFSET						2048

void ext2_init();

#endif // EXT2_H