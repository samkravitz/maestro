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
#include <dirent.h>

int main(int argc, char *argv[])
{
    DIR *dir = opendir("/");
	struct dirent *de;

	while ((de = readdir(dir)) != NULL)
		printf("%s\n", de->d_name);

	return 0;
}
