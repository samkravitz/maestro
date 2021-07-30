/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kmain.c
 * DATE: July 26, 2021
 * DESCRIPTION: Where it all begins
 */
#include <stdint.h>

#include "txtmode.h"

void kmain()
{
    const char *hello = "Hello\nWorld!\n";
    txtmode_puts(hello);
}