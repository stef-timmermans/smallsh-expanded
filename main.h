#ifndef MAIN_H
#define MAIN_H

// Include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

// Extern globals to avoid multiple definitions
extern int last_exit_status;
extern int foreground_only_mode;

/*
A CTRL-Z command from the keyboard sends a SIGTSTP signal to your parent shell
process and all children at the same time (this is a built-in part of Linux).

A child, if any, running as a foreground process must ignore SIGTSTP. Any children
running as background process must ignore SIGTSTP. When the parent process running
the shell receives SIGTSTP. The shell must display an informative message immediately
if it's sitting at the prompt, or immediately after any currently running foreground 
process has terminated. The shell then enters a state where subsequent commands can
no longer be run in the background. In this state, the & operator should simply be
ignored, i.e., all such  commands are run as if they were foreground processes.

If the user sends SIGTSTP again, then your shell will display another informative
message immediately after any currently running foreground process terminates. The
shell then returns back to the normal condition where the & operator is once again
honored for subsequent commands, allowing them to be executed in the background.
*/
void shell_SIGTSTP(int signo);

/*
A CTRL-C command from the keyboard sends a SIGINT signal to the parent process
and all children at the same time (this is a built-in part of Linux).

Your shell, i.e., the parent process, must ignore SIGINT. Any children running
as background processes must ignore SIGINT. A child running as a foreground process
must terminate itself when it receives SIGINT. The parent must not attempt to
terminate the foreground child process; instead the foreground child (if any) must
terminate itself on receipt of this signal. If a child foreground process is killed
by a signal, the parent must immediately print out the number of the signal that
killed it's foreground child process before prompting the user for the next command.
*/
void shell_SIGINT(int signo); // Used with `sa_parent.sa_handler = shell_SIGINT;`


/*
* Handles the SIGTERM signal and exits with code 0.
*/
void shell_SIGTERM(int signpo);

/*
* Helper function to fork a child process and run one of the non
* built-in commands. Takes in the command and its arguments as
* parameters. Any instance of `$$` in the command has already been
* replaced with the current process ID
*/
void execute_command(char* args[512]); // Used with `sa_tstp.sa_handler = shell_SIGTSTP;`

/*
Your program must expand any instance of "$$" in a command into the
process ID of the smallsh itself. Your shell does not otherwise perform 
variable expansion. 
*/
void expand_double_dollars(char* input_command);

/*
* Main function for the shell. This function will loop until the user enters the
* command `exit`. It will then exit the shell. Unless the shell crashes. That would
* also exit the shell.
*/
int main();

#endif
