#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>

#include "utility.h"

#define BUFFER_SIZE 0x200

int is_command(const char* command, char* input)
{
    return strncmp(command, input, strlen(command)) == 0;
}

void process_command_line(char* line)
{
    // allocate a buffer to hold the command line
    char line_buffer[BUFFER_SIZE];
    strncpy(line_buffer, line, BUFFER_SIZE);

    char* command = strtok(line_buffer, " ");

    int space_count = 0;

    // count the number of spaces in the command line
    // we use this to allocate the minimal amount of memory for the array of arguments
    for (int i = 0; command[i] != '\0'; i++)
    {
        if (command[i] == ' ')
        {
            command[i] = '\0';
        }

        space_count++;
    }
    
    // allocate an array of strings to hold the arguments
    // extra space for the command and a null terminator
    char** argv = malloc((space_count + 2) * sizeof(char*));

    char* arg;
    int i = 1; // start at 1 since argv[0] is expected to be the program
    
    // get the rest of the arguments
    while ((arg = strtok(NULL, " ")) != NULL)
    {
        // add the argument to the array
        argv[i] = arg;

        // strip any trailing new line character
        arg[strcspn(arg, "\n")] = 0;

        i++;
    }

    // add the command to the array
    argv[0] = command;

    // add a null terminator to the array
    argv[i] = NULL;

    // strip any trailing new line character from the command
    command[strcspn(command, "\n")] = 0;

    if (is_command("cd", command)) // compare string to "cd"
    {
        // get the argument
        char* path = argv[1];
        
        // attempt to change to the directory; chdir returns 0 on success
        if (chdir(path) < 0)
        {
            // errno is set to ENOENT if the path doesn't exist
            if (errno == ENOENT)
            {
                printf("Error: no such file or directory\n");
            }
        }
    }
    else if (is_command("dir", command))
    {
        // open the current directory
        DIR* dir = opendir(".");

        // check if the directory was opened successfully
        if (dir == NULL)
        {
            printf("Error: could not open current directory\n");
        }

        struct dirent* entry;

        // read each entry in the directory and print its name
        while ((entry = readdir(dir)) != NULL)
        {
            printf("%s\n", entry->d_name);
        }
    }
    else if (is_command("environ", command))
    {
        // environ is an array of strings defined by the C standard library, it contains all environment variables
        extern char** environ;

        // print out all environment variables
        for (int i = 0; environ[i] != NULL; i++)
        {
            printf("%s\n", environ[i]);
        }
    }
    else if (is_command("pause", command))
    {
        // loop forever until getchar() returns a newline character
        while (getchar() != '\n')
        {
        }
    }
    else if (is_command("quit", command))
    {
        exit(0);
    }
    else
    {
        // execute the program
        exec_program(command, argv);
    }

    free(argv);
}

int main(int argc, char** argv)
{
    // allocate buffer for getline
    char* buffer = malloc(BUFFER_SIZE);
    size_t bufferSize = BUFFER_SIZE;

    // allocate buffer for the cwd
    char cwd[PATH_MAX];

    // infinite loop
    while (1)
    {
        // get cwd so we can print it in the prompt
        getcwd(cwd, sizeof(cwd));

        // print out the prompt
        printf("%s $ ", cwd);
        
        // read user input until they press enter
        size_t out = getline(&buffer, &bufferSize, stdin);

        // call our function to process the command line
        process_command_line(buffer);
    }
}
