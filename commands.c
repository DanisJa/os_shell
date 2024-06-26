#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "commands.h"

void execute_rm(char *filename)
{
	if (remove(filename) != 0)
	{
		perror("Error removing file");
	}
}

void execute_cat(char *filename)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		perror("Error opening file");
		return;
	}
	char c;
	while ((c = fgetc(file)) != EOF)
	{
		putchar(c);
	}
	fclose(file);
}

void execute_clear()
{
	system("clear");
}

void execute_cowsay(char *message)
{
	printf(" _______________\n");
	printf("< %s >\n", message);
	printf(" ---------------\n");
	printf("        \\   ^__^\n");
	printf("         \\  (oo)\\_______\n");
	printf("            (__)\\       )\\/\\\n");
	printf("                ||----w |\n");
	printf("                ||     ||\n");
}

void execute_pwd()
{
	char cwd[1024]; // max path length is 256 on windows, not sure about unix/linux systems, but 1024 has a reasonable safety margin
	printf("%s\n", getcwd(cwd, sizeof(cwd)));
}