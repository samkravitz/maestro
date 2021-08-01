/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: klog.h
 * DATE: July 30, 2021
 * DESCRIPTION: kernel logging utilies
 */
#ifndef KLOG_H
#define KLOG_H

#include "common.h"

void klog(const char *);
void klogd(int);
void klogh(int);

#endif // KLOG_H