/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: pmm.h
 * DATE: March 9th, 2022
 * DESCRIPTION: physical memory manager
 * 
 * The base unit a virtual memory manager works with is universally known as a "page".
 * I've seen conflicting resources on what the base unit a physical memory manager works on.
 * "page", "frame", and "block" are used interchangeably to refer to the pmm's analog of a vmm page.
 * So, I've decided to go with "block" here.
 */
#ifndef PMM_H
#define PMM_H

#include <maestro.h>

// size of a physical memory block in bytes
#define BLOCK_SIZE 4096

void pmminit();

#endif    // PMM_H
