# Group 7

myshell: myshell.c utility.h utility.c
	gcc -o myshell myshell.c utility.c

all: myshell

clean:
	rm -f *.o myshell
