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

#include "stdlib.h"
#include "string.h"
#include "txtmode.h"

// klog - general logging function
void klog(const char *msg)
{
	txtmode_puts(msg);
}

// klogd - logs a base 10 integer
void klogd(int x)
{
	char buff[32];
	memset(buff, 0, sizeof(buff));
	itoa(x, buff, 10);
	klog(buff);
}

// klogh - logs a hexadecimal integer
void klogh(int x)
{
	klog("0x");

	char buff[32];
	memset(buff, 0, sizeof(buff));
	itoa(x, buff, 16);
	klog(buff);
}