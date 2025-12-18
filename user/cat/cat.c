/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: user/cat/cat.c
 * DATE: December 12, 2025
 * DESCRIPTION: cat - concatenate and display files
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: cat <file>\n");
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
	char buffer[1024];
	read(fd, buffer, 1024);
	printf("%s\n", buffer);
	close(fd);
	return 0;
}
