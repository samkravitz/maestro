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
#include <unistd.h>

char line[1024];

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	int y = 0x70;
	int x = y - 1;
	int *px = &x;
	*px = 0x420;

	int pid = fork();

	if (pid == 0)
	{
		//while (1)
		//	;
		printf("child\n");
	}

	else if (pid > 0) {
		printf("adult %d\n", pid);
	}

	else {
		printf("err %d\n", pid);
	}


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
