#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "utility.h"

void exec_program(char* program, char** argv, FILE* out, bool background)
{
    // fork a new process
    pid_t pid = fork();

    if (pid == 0) // child process
    {
        // transfer SHELL environment variable to PARENT
        char* shell = getenv("SHELL");
        setenv("PARENT", shell, true);

        // set stdout to the output file
        dup2(fileno(out), STDOUT_FILENO);

        // execute the program
        execvp(program, argv);

        // if we get here, execvp failed
        printf("Error: failed to execute program %s\n", program);
        exit(1);
    }
    else // parent process
    {
        if (!background)
        {
            // wait for the child process to finish
            wait(NULL);
        }
    }
}

void get_executable_path(char* buffer, int buffer_size, char* argv0)
{
    if (argv0[0] == '/') // absolute path
    {
        strncpy(buffer, argv0, buffer_size);
    }
    else // relative path
    {
        // get the current working directory
        getcwd(buffer, buffer_size);

        // append the relative path to the executable
        strncat(buffer, "/", buffer_size);
        strncat(buffer, argv0, buffer_size);
    }
}
