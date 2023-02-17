#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>

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

    // Parse the command line for redirection and background execution
    FILE* out_file = stdout;
    bool background = false;

    for (int j = i - 1; j != 0; j--)
    {
        printf("arg %d: %s\n", j, argv[j]);

        if (argv[j][0] == '&')
        {
            argv[j] = NULL;
            background = true;
        }
        else if (argv[j][0] == '>')
        {
            char* mode;

            if (argv[j][1] == '>') // double >> is append
            {
                mode = "a+";
            }
            else // single > is truncate
            {
                mode = "w+";
            }

            out_file = fopen(argv[j + 1], mode);

            if (out_file == NULL)
            {
                printf("Error: could not open file %s\n", argv[j + 1]);
                return;
            }
            
            // POSIX standard doesn't specify initial file position for append mode
            // always seek to the end of the file
            fseek(out_file, 0, SEEK_END);

            argv[j] = NULL;
        }
    }

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
        exec_program(command, argv, out_file, background);
    }

    if (out_file != stdout)
    {
        fclose(out_file);
    }

    free(argv);
}

int main(int argc, char** argv)
{
    // allocate buffer for getline
    char buffer[BUFFER_SIZE];
    char* bufferPtr = &buffer[0];
    size_t bufferSize = BUFFER_SIZE;

    // set the SHELL environment variable
    get_executable_path(buffer, BUFFER_SIZE, argv[0]);
    setenv("SHELL", buffer, true);

    // allocate buffer for the cwd
    char cwd[PATH_MAX];

    // the fd to read input from
    FILE* input_file = stdin;
    bool is_running_script = false;

    if (argc == 2)
    {
        input_file = fopen(argv[1], "r");
        is_running_script = true;
        
        if (input_file == NULL)
        {
            printf("Error: could not open file %s\n", argv[1]);
            exit(1);
        }
    }

    // infinite loop
    while (1)
    {
        if (!is_running_script)
        {
            // get cwd so we can print it in the prompt
            getcwd(cwd, sizeof(cwd));

            // print out the prompt
            printf("%s $ ", cwd);
        }
        
        // read user input until they press enter
        size_t out = getline(&bufferPtr, &bufferSize, input_file);

        // check if an error occurred (including eof)
        if (out == -1)
        {
            break;
        }

        // call our function to process the command line
        process_command_line(buffer);
    }

    if (is_running_script)
    {
        // close the file if we opened one
        fclose(input_file);
    }
}
