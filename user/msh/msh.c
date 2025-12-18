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
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char line[1024];

static char **split_line(char *);
static void run_command(char **args);

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

		line[pos] = '\0';
		char **args = split_line(line);
		if (args[0] == NULL)
		{
			printf("Error parsing command: %s\n", line);
			free(args);
			continue;
		}

		// Edge case: command is "exit"
		if (strcmp(args[0], "exit") == 0)
		{
			free(args);
			return 0;
		}

		// Fork a new process to run the command
		run_command(args);

		printf("\n");
	}

	return 0;
}

static char **split_line(char *line)
{
	int bufsize = 64, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (!tokens)
	{
		printf("msh: allocation error\n");
		exit(1);
	}

	token = strtok(line, " ");
	while (token != NULL)
	{
		tokens[position++] = token;

		if (position >= bufsize)
		{
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens)
			{
				printf("msh: allocation error\n");
				exit(1);
			}
		}

		token = strtok(NULL, " ");
	}
	tokens[position] = NULL;
	return tokens;
}

static void run_command(char **args)
{
	pid_t pid = fork();
	if (pid < 0)
	{
		printf("msh: fork failed\n");
		exit(1);
	}

	// child
	else if (pid == 0)
	{
		execv(args[0], args);
		printf("msh: command not found: %s\n", args[0]);
		exit(1);
	}

	else
	{
		// parent
		int status;
		waitpid(pid, &status, 0);
	}
}
