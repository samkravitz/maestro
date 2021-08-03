/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: kmain.c
 * DATE: July 26, 2021
 * DESCRIPTION: Where it all begins
 */
#include "init.h"
#include "klog.h"

void kmain()
{
    klog("Welcome to maestro!\n");
    init();

    // hang so interrupts remain enabled
    while (1);
}