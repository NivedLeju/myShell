#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>

int is_command(const char* command, char* input)
{
    return strncmp(command, input, strlen(command)) == 0;
}

void process_command_line(char* line)
{
    char* command = strtok(line, " ");

    if (is_command("cd", command)) // compare string to "cd"
    {
        // get the argument
        char* path = strtok(NULL, " ");
        
        if (path == NULL)
        {
            printf("Error: no path specified\n");
            return;
        }

        // strip trailing new line character
        path[strcspn(path, "\n")] = 0;

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
    else if (is_command("exit", command))
    {
        exit(0);
    }
}

int main(int argc, char** argv)
{
    // allocate buffer for getline
    char* buffer = malloc(100);
    size_t bufferSize = 100;

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
