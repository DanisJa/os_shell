#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "commands.h"

void execute_rm(char *filename){
	if(remove(filename) != 0){
		perror("Error removing file");
	}
}

void execute_cat(char *filename){
	FILE *file = fopen(filename, "r");
	if(file == NULL){
		perror("Error opening file");
		return;
	}
	char c;
	while ((c = fgetc(file)) != EOF) {
		putchar(c);
	}
	fclose(file);
}

void execute_clear(){
	system("clear");
}

void execute_cowsay(char *message) {
	char command[100];
	snprintf(command, sizeof(command), "cowsay \"%s\"", message);
	system(command);
}
