/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: user/ls/ls.c
 * DATE: March 28th, 2022
 * DESCRIPTION: ls - list directory contents
 */

#include "ls.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    printf("argc = %d\n", argc);
    printf("%x\n", *argv);
    for (int i = 0; i < argc; i++) {
        printf("argv = %s %x\n", argv[i], &argv[i]);
    }
        

	printf("Hello world!\n");
	return 0;
}
