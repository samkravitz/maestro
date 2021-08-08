/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kmain.c
 * DATE: July 26, 2021
 * DESCRIPTION: Where it all begins
 */
#include <init.h>
#include <kout.h>

void kmain()
{
	kout("Welcome to maestro!\n");
	init();

	// hang so interrupts remain enabled
	while (1);
}