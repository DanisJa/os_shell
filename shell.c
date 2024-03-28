#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "commands.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64

// colors
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"

int in_pipe = 0;
int pipes[2];

void execute_command(char *command, char **args)
{
	if (strcmp(command, "rm") == 0)
	{
		if (!args[0])
		{
			fprintf(stderr, COLOR_RED "Usage: rm <filename>\n" COLOR_RESET);
			exit(EXIT_FAILURE);
		}
		execute_rm(args[0]);
	}
	else if (strcmp(command, "cat") == 0)
	{
		if (!args[0])
		{
			fprintf(stderr, COLOR_RED "Usage: cat <filename>\n" COLOR_RESET);
			exit(EXIT_FAILURE);
		}
		execute_cat(args[0]);
	}
	else if (strcmp(command, "clear") == 0)
	{
		execute_clear();
	}
	else if (strcmp(command, "cowsay") == 0)
	{
		if (!args[0])
		{
			fprintf(stderr, COLOR_RED "Usage: cowsay <message>\n" COLOR_RESET);
			exit(EXIT_FAILURE);
		}
		execute_cowsay(args[0]);
	}
	else if (strcmp(command, "pwd") == 0)
	{
		printf(COLOR_BLUE);
		execute_pwd();
		printf(COLOR_RESET);
	}
	else
	{
		fprintf(stderr, COLOR_RED "Unknown command: %s\n" COLOR_RESET, command);
		exit(EXIT_FAILURE);
	}
}

int main()
{
	char command[MAX_COMMAND_LENGTH];
	char *args[MAX_ARGS];

	while (1)
	{
		// Prompt
		char machinename[1024];
		if (gethostname(machinename, sizeof(machinename)) != 0 || !getlogin())
		{
			perror("Failed to get hostname or login");
			exit(EXIT_FAILURE);
		}
		printf(COLOR_GREEN "%s@%s: ~$ " COLOR_RESET, machinename, getlogin());

		if (!fgets(command, sizeof(command), stdin))
			break;
		if (command[0] == '\n')
			continue;

		in_pipe = 0;
		int arg_count = 0;
		// Tokenizing
		char *token = strtok(command, " \n");
		while (token != NULL && arg_count < MAX_ARGS - 1)
		{
			if (strcmp(token, "|") == 0)
			{
				args[arg_count++] = NULL;
				in_pipe = 1;
			}
			else
				args[arg_count++] = token;
			token = strtok(NULL, " \n");
		}
		args[arg_count] = NULL;

		if (in_pipe)
		{
			if (pipe(pipes) < 0)
			{
				perror("Pipe failed");
				exit(EXIT_FAILURE);
			}
			pid_t child_pid = fork();
			if (child_pid < 0)
			{
				perror("Fork failed");
				exit(EXIT_FAILURE);
			}
			else if (child_pid == 0)
			{
				if (in_pipe)
				{
					close(pipes[0]);							 // close read end of the pipe
					dup2(pipes[1], STDOUT_FILENO); // redirect stdout to the write end of the pipe
					close(pipes[1]);							 // close the write end of the pipe
				}
				execute_command(args[0], args); // execute cat or cowsay
				exit(EXIT_SUCCESS);
			}
			else
			{
				if (in_pipe)
				{
					close(pipes[1]);							// close the write end of the pipe
					dup2(pipes[0], STDIN_FILENO); // redirect stdin to the read end of the pipe
					close(pipes[0]);							// close the read end of the pipe
				}
				wait(NULL); // wait for the child process to finish
			}
			// printf(COLOR_BLUE "%s " COLOR_GREEN "%s " COLOR_RED "%s\n" COLOR_RESET, "piping", "not yet", "implemented");
		}
		else
		{
			pid_t child_pid = fork();
			if (child_pid < 0)
			{
				perror("Fork failed");
				exit(EXIT_FAILURE);
			}
			else if (child_pid == 0)
			{
				execute_command(args[0], args + 1); // Execute the command
				exit(EXIT_SUCCESS);
			}
			else
			{
				wait(NULL); // Wait for the child process to finish
			}
		}
	}

	return 0;
}
