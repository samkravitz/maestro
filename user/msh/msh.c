/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: user/msh/msh.c
 * DATE: March 13th, 2023
 * DESCRIPTION: maestro shell
 */

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

		printf("\n");

		if (!strcmp("exit", line))
			break;

		if (!strcmp("ls", line))
		{
			DIR *dir = opendir("/");
			struct dirent *de;

			while ((de = readdir(dir)) != NULL)
				printf("%s\n", de->d_name);
		}

		if (strncmp(line, "cat ", 4) == 0)
		{
			char *filename = line + 4;
			int fd = open(filename, O_RDONLY);
			if (fd < 0)
			{
				printf("cat: %s: No such file\n", filename);
			}

			else
			{
				char buf[512];
				int n = read(fd, buf, sizeof(buf) - 1);
				printf("%s", buf);
			}
		}

		printf("\n");
	}

	return 0;
}
