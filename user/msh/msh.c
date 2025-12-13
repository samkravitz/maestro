/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: user/msh/msh.c
 * DATE: March 13th, 2023
 * DESCRIPTION: maestro shell
 */

#include <stdio.h>
#include <string.h>

char line[1024];

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	while (1)
	{
		memset(line, 0, 1024);
		printf("> ");
		int pos = 0;

		while (1)
		{
			int c = getc();
			if (c == '\n')
				break;

			line[pos++] = c;
			printf("%c", c);
		}

		if (!strcmp("exit", line))
			break;

		printf("\n");
	}

	return 0;
}
