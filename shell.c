#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "commands.h"

#define MAX_COMMAND_LENGTH	1024
#define MAX_ARGS		64

int main(){
	int should_run = 1;
	char command[MAX_COMMAND_LENGTH];
	char *args[MAX_ARGS];
	
	while(should_run){
		//prompt display
		char machinename[1024];

		if(gethostname(machinename, sizeof(machinename)) != 0){
			perror("gethostname");
			exit(EXIT_FAILURE);
		}

		char *username = getlogin();
		if(username == NULL){
			perror("getlogin");
			exit(EXIT_FAILURE);
		}
		
		printf("%s@%s: ~$ ", machinename, username);

		if(fgets(command, sizeof(command), stdin) == NULL){
			break;
		}

		// parsing the commands
		char *token;
		int arg_count = 0;
		token = strtok(command, " \n");
		while(token != NULL && arg_count < MAX_ARGS - 1) {
			args[arg_count] = token;
			arg_count++;
			token = strtok(NULL, " \n");
		}
		args[arg_count] = NULL;

		if (strcmp(args[0], "rm") == 0) {
			if(arg_count < 2){
				fprintf(stderr, "Usage: rm <filename>>\n");
			} else {
				execute_rm(args[1]);
			}
		} else if (strcmp(args[0], "cat") == 0){
			if(arg_count < 2){
				fprintf(stderr, "Usage: cat <filename>\n");
			} else {
				execute_cat(args[1]);
			}
		} else if (strcmp(args[0], "clear") == 0){
			execute_clear();
		} else if (strcmp(args[0], "cowsay") == 0){
			if(arg_count < 2){
				fprintf(stderr, "Usage: cowsay <message>\n");
			} else {
				execute_cowsay(args[1]);
			}
		} 
		
	}
}
