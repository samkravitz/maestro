/* kernel
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/string.c
 * DATE: July 27, 2021
 * DESCRIPTION: strlen, 
 */

#include "string.h"

// compute the length of a null-terminated string
size_t strlen(const char *str)
{
    size_t len = 0;
    while (*str++)
        len++;
    
    return len;
}