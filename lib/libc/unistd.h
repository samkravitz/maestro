/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/unistd.h
 * DATE: April 6th, 2022
 * DESCRIPTION: POSIX compatibility definitions
 */

#ifndef UNISTD_H
#define UNISTD_H

#include <maestro.h>

size_t read(int, void *, size_t);
size_t write(int, void *, size_t);

#endif    // UNISTD_H
