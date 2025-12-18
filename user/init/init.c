/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: user/init/init.c
 * DATE: December 17, 2025
 * DESCRIPTION: init - The first user-space process that spawns the shell
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;
	
	while (1)
	{
		int pid = fork();

		// child - exec the shell
		if (pid == 0)
		{
			execv("/bin/msh", (char *[]) { "/bin/msh", NULL });
			printf("init: failed to exec /bin/msh\n");
			exit(1);
		}

		// parent: wait for shell to exit
		else if (pid > 0)
		{
			int status;
			waitpid(pid, &status, 0);

			printf("init: shell exited with status %d, respawning...\n", status);
		}

		else
		{
			printf("init: fork failed\n");
			exit(1);
		}
	}

	return 0;
}
