#pragma once

// executes a program with the given arguments
void exec_program(char* program, char** argv);

// gets the path of the executable
void get_executable_path(char* buffer, int buffer_size, char* argv0);
