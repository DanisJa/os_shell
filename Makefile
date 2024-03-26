CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: shell

shell: shell.o commands.o
	$(CC) $(CFLAGS) -o shell shell.o commands.o
    
shell.o: shell.c commands.h
	$(CC) $(CFLAGS) -c shell.c

commands.o: commands.c commands.h
	$(CC) $(CFLAGS) -c commands.c

clean: 
	rm -f *.o shell

