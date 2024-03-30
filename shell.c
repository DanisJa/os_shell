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

void execute_command(char *command, char **args);

void print_command_and_arguments(char *command, char **args, int argc){
    printf("%s %d\n", "Argument count:", argc);
    printf("%s %s\n", "Command: ", command);
    for (int i = 0; i < argc - 1; i++) {
        printf("%s %d %s\n", "Argument: ", i, args[i]);
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];

    while (1) {
        // Prompt
        char machinename[1024];
        if (gethostname(machinename, sizeof(machinename)) != 0 || !getlogin()) {
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
        while (token != NULL && arg_count < MAX_ARGS - 1) {
            if (strcmp(token, "|") == 0) {
                args[arg_count++] = NULL; // marks end of current command
                in_pipe = 1; // sets in_pipe state to 1
                arg_count = 0; // resets the count, so we can count args for next command
            } else
                args[arg_count++] = token;
            token = strtok(NULL, " \n");
        }
        args[arg_count] = NULL;

        // Executing command with(out) pipe
//        if (in_pipe) {
//            printf("in pipe");
//            if (pipe(pipes) < 0) {
//                perror("Pipe failed");
//                exit(EXIT_FAILURE);
//            }
//            pid_t child_pid = fork();
//            if (child_pid < 0) {
//                perror("Fork failed");
//                exit(EXIT_FAILURE);
//            } else if (child_pid == 0) {
//                close(pipes[0]);                             // close read end of the pipe
//                dup2(pipes[1], STDOUT_FILENO); // redirect stdout to the write end of the pipe
//                close(pipes[1]);                             // close the write end of the pipe
//                execute_command(args[0], args);
//                printf("%s %s\n", args[0], args[1]);
//            } else {
//                close(pipes[1]);                            // close the write end of the pipe
//                dup2(pipes[0], STDIN_FILENO); // redirect stdin to the read end of the pipe
//                close(pipes[0]);                            // close the read end of the pipe
//
//                execute_command(args[arg_count + 1], args + arg_count + 2);
//                wait(NULL); // wait for the child process to finish
//            }
//        }
////             Executing command with fork() without piping
//        else {
            pid_t child_pid = fork();
            if (child_pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            } else if (child_pid == 0) {
                execute_command(args[0], args + 1);
                exit(EXIT_SUCCESS);
            } else {
                wait(NULL);
            }
        }
//    }
    return 0;
}

    void execute_command(char *command, char **args) {
        if (strcmp(command, "rm") == 0) {
            if (!args[0]) {
                fprintf(stderr, COLOR_RED "Usage: rm <filename>\n" COLOR_RESET);
                exit(EXIT_FAILURE);
            }
            execute_rm(args[0]);
        } else if (strcmp(command, "cat") == 0) {
            if (!args[0]) {
                fprintf(stderr, COLOR_RED "Usage: cat <filename>\n" COLOR_RESET);
                exit(EXIT_FAILURE);
            }
            execute_cat(args[0]);
            // Printing newline because prompt should be under the file contents, too lazy to find other solutions
            printf("\n");
        } else if (strcmp(command, "clear") == 0) {
            execute_clear();
        } else if (strcmp(command, "cowsay") == 0) {
            if (!args[0]) {
                fprintf(stderr, COLOR_RED "Usage: cowsay <message>\n" COLOR_RESET);
                exit(EXIT_FAILURE);
            }
            execute_cowsay(args[0]);
        } else if (strcmp(command, "pwd") == 0) {
            printf(COLOR_BLUE);
            execute_pwd();
            printf(COLOR_RESET);
        } else {
            fprintf(stderr, COLOR_RED "Unknown command: %s\n" COLOR_RESET, command);
            exit(EXIT_FAILURE);
        }
    }
