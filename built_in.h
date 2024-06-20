#ifndef BUILT_IN_H
#define BUILT_IN_H

#include "main.h"

// Define the maximum number of processes
#define MAX_PROCESSES 10

// Track the background processes
int processes_count;
int processes[MAX_PROCESSES];

/*
Your shell will support three built-in commands: exit, cd, and status.
These three built-in commands are the only ones that your shell will handle
itself - all others are simply passed on to a member of the exec() family
of functions.

The three built-in shell commands do not count as foreground processes
for the purposes of this built-in command - i.e., status should ignore
built-in commands.
*/

/*
The exit command exits your shell. It takes no arguments. When this command
is run, your shell must kill any other processes or jobs that your shell has
started before it terminates itself.
*/
void exit_shell();

/*
The cd command changes the working directory of smallsh.

By itself - with no arguments - it changes to the directory specified in the
HOME environment variable. This is typically not the location where smallsh
was executed from, unless your shell executable is located in the HOME directory,
in which case these are the same. This command can also take one argument: the 
path of a directory to change to. Your cd command should support both absolute
and relative paths.
*/
void cd_shell(char* args[512]);

/*
Implements the environment variable call to print the current working directory
to stdout. This command takes no arguments.
*/
void pwd_shell();

/*
The status command prints out either the exit status or the terminating signal
of the last foreground process ran by your shell.

If this command is run before any foreground command is run, then it should
simply return the exit status 0. The three built-in shell commands do not count
as foreground processes for the purposes of this built-in command - i.e., status
should ignore built-in commands.
*/
void status_shell(char* args[512]);

#endif
