/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: user/msh/msh.c
 * DATE: March 28th, 2022
 * DESCRIPTION: maestro shell
 */

#include "msh.h"

#include "stdio.h"
#include "string.h"

u8 line[1024];

int msh()
{
	while (1)
	{
		memset(line, 0, 1024);
		printf("> ");
		int c, pos = 0;
		while ((c = getc()) != '\n')
		{
			line[pos++] = c;
			printf("%c", c);
		}

		printf("\n");
	}
	return 0;
}
