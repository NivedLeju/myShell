#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "utility.h"

void exec_program(char* program, char** argv)
{
    // fork a new process
    pid_t pid = fork();

    if (pid == 0) // child process
    {
        // execute the program
        execvp(program, argv);

        // if we get here, execvp failed
        printf("Error: failed to execute program %s\n", program);
        exit(1);
    }
    else // parent process
    {
        // wait for the child process to finish
        wait(NULL);
    }
}
