myshell: myshell.c
	gcc -o myshell myshell.c

all: myshell

clean:
	rm -f *.o myshell
