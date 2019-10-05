CC	= gcc
CFLAGS	= -Wall -O2

.PHONY: all clean compile compress

all: dragonshell

clean:
	rm *.o dragonshell

compile: dragonshell.c
	$(CC) $(CFLAGS) -c dragonshell.c -o dragonshell.o

dragonshell: dragonshell.o
	$(CC) $(CFLAGS) -o dragonshell dragonshell.o
	
compress:
	zip dragonshell.zip dragonshell.c Makefile README.md
