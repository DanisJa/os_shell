#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "commands.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64 // for future implementation of n args

int main()
{
	int should_run = 1; // just send interrupt signal for now (or maybe forever)
	char command[MAX_COMMAND_LENGTH];
	char *args[MAX_ARGS];

	while (should_run)
	{
		// prompt display
		char machinename[1024];

		if (gethostname(machinename, sizeof(machinename)) != 0)
		{
			perror("gethostname");
			exit(EXIT_FAILURE);
		}

		char *username = getlogin();
		if (username == NULL)
		{
			perror("getlogin");
			exit(EXIT_FAILURE);
		}

		printf("%s@%s: ~$ ", machinename, username);

		if (fgets(command, sizeof(command), stdin) == NULL)
		{
			break;
		}

		if (command[0] == '\n')
		{
			continue;
		}

		// parsing the commands
		char *token;
		int arg_count = 0;
		token = strtok(command, " \n");
		while (token != NULL && arg_count < MAX_ARGS - 1)
		{
			args[arg_count] = token;
			arg_count++;
			token = strtok(NULL, " \n");
		}
		args[arg_count] = NULL;

		// checking for pipes
		int i;
		int num_pipes = 0;

		while (token != NULL && arg_count < MAX_ARGS - 1)
		{
			args[arg_count] = token;
			arg_count++;
			token = strtok(NULL, " \n");
		}

		args[arg_count] = NULL;

		// calling (or throwing errors) the function for respective commands
		if (num_pipes == 0) // NO PIPES
		{
			if (strcmp(args[0], "rm") == 0)
			{
				if (arg_count < 2)
				{
					fprintf(stderr, "Usage: rm <filename>>\n");
				}
				else
				{
					execute_rm(args[1]);
				}
			}
			else if (strcmp(args[0], "cat") == 0)
			{
				if (arg_count < 2)
				{
					fprintf(stderr, "Usage: cat <filename>\n");
				}
				else
				{
					execute_cat(args[1]);
				}
			}
			else if (strcmp(args[0], "clear") == 0)
			{
				execute_clear();
			}
			else if (strcmp(args[0], "cowsay") == 0)
			{
				if (arg_count < 2)
				{
					fprintf(stderr, "Usage: cowsay <message>\n");
				}
				else
				{
					execute_cowsay(args[1]);
				}
			}
		}
		else
		{ // piping
			int pipes[2];
			int prev_pipe_read = STDIN_FILENO;

			for (i = 0; i < num_pipes; i++)
			{
				pipe(pipes);
				pid_t childp = fork();

				if (childp < 0)
				{
					perror("fork failed");
					exit(EXIT_FAILURE);
				}
				else if (childp == 0) // child process goes here
				{
					close(pipes[0]); // closing unused end

					if (i < num_pipes)
					{
						if (dup2(pipes[1], STDOUT_FILENO) < 0)
						{
							perror("dup2 fail");
							exit(EXIT_FAILURE);
						}
					}

					if (dup2(prev_pipe_read, STDIN_FILENO) < 0)
					{
						perror("dup2 fail");
						exit(EXIT_FAILURE);
					}

					close(prev_pipe_read);
					close(pipes[1]);

					execvp(args[i * 2], args + (i * 2));
					perror("execvp");
					exit(EXIT_FAILURE);
				}
				else
				{
					close(pipes[1]);					 // close unused write end of pipe
					wait(NULL);								 // wait for child to end
					prev_pipe_read = pipes[0]; // next command reads from this
				}
			}
		}
	}
}
