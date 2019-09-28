CC	= gcc
CFLAGS	= -Wall -O2

.PHONY: all clean

all: dragonshell

clean:
	rm *.o dragonshell

dragonshell.o: dragonshell.c
	$(CC) $(CFLAGS) -c dragonshell.c -o dragonshell.o

dragonshell: dragonshell.o
	$(CC) $(CFLAGS) -o dragonshell dragonshell.o
