// Group 7

#pragma once

#include <stdbool.h>
#include <stdio.h>

// executes a program with the given arguments
void exec_program(char* program, char** argv, FILE* in, FILE* out, bool background);

// gets the path of the executable
void get_executable_path(char* buffer, int buffer_size, char* argv0);
