#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

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
	}
}
