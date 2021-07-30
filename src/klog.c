/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: klog.c
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#include "klog.h"

#include "txtmode.h"

void klog(const char *msg)
{
	txtmode_puts(msg);
}