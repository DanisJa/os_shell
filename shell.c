#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "commands.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64

void execute_command(char *command, char **args)
{
	if (strcmp(command, "rm") == 0)
	{
		if (!args[0])
		{
			fprintf(stderr, "Usage: rm <filename>\n");
			exit(EXIT_FAILURE);
		}
		execute_rm(args[0]);
	}
	else if (strcmp(command, "cat") == 0)
	{
		if (!args[0])
		{
			fprintf(stderr, "Usage: cat <filename>\n");
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
			fprintf(stderr, "Usage: cowsay <message>\n");
			exit(EXIT_FAILURE);
		}
		execute_cowsay(args[0]);
	}
	else if (strcmp(command, "pwd") == 0)
	{
		system("pwd");
	}
	else
	{
		fprintf(stderr, "Unknown command: %s\n", command);
		exit(EXIT_FAILURE);
	}
}

int main()
{
	char command[MAX_COMMAND_LENGTH];
	char *args[MAX_ARGS];

	while (1)
	{
		char machinename[1024];
		if (gethostname(machinename, sizeof(machinename)) != 0 || !getlogin())
		{
			perror("Failed to get hostname or login");
			exit(EXIT_FAILURE);
		}
		printf("%s@%s: ~$ ", machinename, getlogin());

		if (!fgets(command, sizeof(command), stdin))
			break;
		if (command[0] == '\n')
			continue;

		int in_pipe = 0;
		int arg_count = 0;
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
			int pipes[2];
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
				close(pipes[0]);								// Close read end of the pipe
				dup2(pipes[1], STDOUT_FILENO);	// Redirect stdout to the write end of the pipe
				close(pipes[1]);								// Close the write end of the pipe
				execute_command(args[0], args); // Execute the command
				exit(EXIT_SUCCESS);
			}
			else
			{
				close(pipes[1]);							// Close the write end of the pipe
				dup2(pipes[0], STDIN_FILENO); // Redirect stdin to the read end of the pipe
				close(pipes[0]);							// Close the read end of the pipe
				wait(NULL);										// Wait for the child process to finish
			}
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
