#pragma once

#include <stdbool.h>

// executes a program with the given arguments
void exec_program(char* program, char** argv, bool background);

// gets the path of the executable
void get_executable_path(char* buffer, int buffer_size, char* argv0);
