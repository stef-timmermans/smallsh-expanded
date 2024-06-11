/* 
* Program file: main.c
* Author: Stef Timmermans
* Date: 26 May 2023
* Description:
*   This program executes the main program for CS 344
*   Assignment 3: smallsh. The program takes in a command
*   in one of the following formats and executes it:
*     
*     command [arg1 arg2 ...] [< input_file] [> output_file] [&]
*
*   This will be done by creating both the built-in commands (exit, 
*   cd, and status) and the non-built-in commands (all other commands
*   which will be executed by a child process). The program will also
*   have custom handlers for two signals: SIGINT and SIGTSTP.
*/

#include "built_in.h"

// Global variables for last exit status and foreground pid
int last_exit_status = 0;
int foreground_only_mode = 0;

void shell_SIGTSTP(int signo)
{
    if (foreground_only_mode)
    {
        // Exiting foreground-only mode
        char* message = "\nExiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 28);
    }
    else
    {
        // Entering foreground-only mode
        char* message = "\nEntering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 51);
    }

    // ChatGPT recommended adding this toggle to stop an error with background processes
    foreground_only_mode = !foreground_only_mode;
}

void shell_SIGINT(int signo)
{
    // Ignore SIGINT in the parent shell (this does nothing)
}

// Debugged an error with ChatGPT, which suggested a new function for SIGTERM signals
// This wasn't specifically required by the rubric, but it was good to catch this error
void shell_SIGTERM(int signpo)
{
    // Handle the SIGTERM signal (terminate the process)
    exit(0);
}

/*
* Function creates a new process to execute the command
* Researched more about forking a process here:
*   https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-waitpid-wait-specific-child-process-end
* Researched more about the usage of execvp here:
*   https://support.sas.com/documentation/onlinedoc/ccompiler/doc750/html/lr2/zid-7307.htm
*/
void execute_command(char* args[512])
{
    int status;
    pid_t pid = fork();

    // Check for input/output redirection
    int inRedirectIndex = -1;
    int outRedirectIndex = -1;

    // Look for '>' and '<' symbols in arguments
    int i;
    for (i = 0; args[i] != NULL; i++)
    {
        // Found input redirection symbol '<'
        if (strcmp(args[i], "<") == 0)
        {
            inRedirectIndex = i;
        }

        // Found output redirection symbol '>'
        if (strcmp(args[i], ">") == 0)
        {
            outRedirectIndex = i;
        }
    }

    if (pid == -1)
    {
        perror("fork failed!");
    }
    else if
    (pid == 0)
    {
        // In child process!

        // Set up signal handling for the child process
        // Use sigaction struct calls from lecture
        // sigaction from the `signal.h` header file
        struct sigaction sa_child = {0};
        sa_child.sa_handler = SIG_DFL;
        sigfillset(&sa_child.sa_mask);
        sa_child.sa_flags = SA_RESTART; 
        sigaction(SIGINT, &sa_child, NULL);

        struct sigaction sa_child_tstp = {0};
        sa_child_tstp.sa_handler = SIG_IGN;
        sigfillset(&sa_child_tstp.sa_mask);
        sa_child_tstp.sa_flags = SA_RESTART;
        sigaction(SIGTSTP, &sa_child_tstp, NULL);

        // Input redirection
        if (inRedirectIndex != -1)
        {
            // Input only needs to be read, so open in read-only mode
            int inputFile = open(args[inRedirectIndex + 1], O_RDONLY);
            
            if (inputFile == -1)
            {
                perror("input file open");
                exit(EXIT_FAILURE);
            }

            if (dup2(inputFile, STDIN_FILENO) == -1)
            {
                perror("input dup2");
                exit(EXIT_FAILURE);
            }

            close(inputFile);

            // Remove arguments "<" and the following argument
            args[inRedirectIndex] = NULL;
        }

        // Output redirection
        if (outRedirectIndex != -1)
        {
            // Output only needs to be written, so open in write-only mode
            int outputFile = open(args[outRedirectIndex + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (outputFile == -1)
            {
                perror("output file open");
                exit(EXIT_FAILURE);
            }

            if (dup2(outputFile, STDOUT_FILENO) == -1)
            {
                perror("output dup2");
                exit(EXIT_FAILURE);
            }

            close(outputFile);

            // Remove arguments ">" and the following argument
            args[outRedirectIndex] = NULL;
        }

        // Check if in foreground-only mode
        if (foreground_only_mode) {
            // Redirect standard input and output to /dev/null
            int nullFile = open("/dev/null", O_WRONLY);
            if (nullFile == -1)
            {
                perror("open /dev/null");
                exit(EXIT_FAILURE);
            }

            if (dup2(nullFile, STDIN_FILENO) == -1)
            {
                perror("dup2 /dev/null");
                exit(EXIT_FAILURE);
            }

            if (dup2(nullFile, STDOUT_FILENO) == -1)
            {
                perror("dup2 /dev/null");
                exit(EXIT_FAILURE);
            }

            close(nullFile);
        }

        // Sleep command
        if (strcmp(args[0], "sleep") == 0)
        {
            if (args[1] != NULL)
            {
                int duration = atoi(args[1]);
                sleep(duration);
            }
        }
        else
        {
            execvp(args[0], args);

            // The below code should never be reached, unless exec fails
            perror("execvp failed!");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // In parent process! (pid == child's pid)
        if (!foreground_only_mode)
        {
            do {
                pid_t w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
                if (w == -1)
                {
                    perror("waitpid failed!");
                    exit(EXIT_FAILURE);
                }

                if (WIFEXITED(status))
                {
                    last_exit_status = WEXITSTATUS(status);
                }
                else if (WIFSIGNALED(status))
                {
                    printf("\nTerminated by signal %d\n", WTERMSIG(status));
                    fflush(stdout);
                    last_exit_status = WTERMSIG(status);
                }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        else
        {
            // Print process ID and exit value after sleep command
            printf("background pid %d is done: exit value %d\n", pid, last_exit_status);
            fflush(stdout);
        }
    }
}

void expand_double_dollars(char* input_command)
{
    char pid[8];
    sprintf(pid, "%d", getpid());

    char* pos;
    while ((pos = strstr(input_command, "$$")) != NULL)
    {
        size_t posIndex = pos - input_command;

        char newStr[2048]; // Create a new string that replaces "$$" with the pid
        strncpy(newStr, input_command, posIndex); // Copy the content before "$$"
        newStr[posIndex] = '\0'; // Ensure the string is null-terminated

        // Concatenate pid and the rest of the original string
        strcat(newStr, pid);
        strcat(newStr, pos + 2);

        // Copy the new string back to the original string
        strcpy(input_command, newStr);
    }
}

int main()
{
    /*
    * Setup the signal handlers for SIGINT, SIGTSTP, and SIGTERM
    * This is done via structs and sigaction()
    * Reference: https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
    *   Had ChatGPT help with the ordering for assigning values to the structs and dealing
    *   with them in execute_command().
    */

    // Ignore SIGINT in parent shell
    struct sigaction sa_parent = {0};
    sa_parent.sa_handler = shell_SIGINT;
    sigfillset(&sa_parent.sa_mask);
    sa_parent.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_parent, NULL);

    // Handle SIGTSTP in parent shell
    struct sigaction sa_tstp = {0};
    sa_tstp.sa_handler = shell_SIGTSTP;
    sigfillset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa_tstp, NULL);

    // Handle SIGTERM in parent shell
    struct sigaction sa_term = {0};
    sa_term.sa_handler = shell_SIGTERM;
    sigfillset(&sa_term.sa_mask);
    sa_term.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sa_term, NULL);

    int debug = 0;

    char input[2048];
    char* args[512];
    int status = 0;

    while(1)
    {
        printf(": "); // Shell prompt ": ""
        fflush(stdout); // Flush the output buffer
        fgets(input, 2048, stdin); // Get line of input from user
        size_t inputLength = strlen(input);
        if (inputLength > 0 && input[inputLength - 1] == '\n')
        {
            input[inputLength - 1] = '\0'; // Replace newline character with null terminator
        }

        expand_double_dollars(input);

        // Populate the args array with the input command ($$ expanded)
        char* token;
        int i = 0;
        token = strtok(input, " \n"); // Space and newline delimiters
        while (token != NULL)
        {
            args[i] = token;
            token = strtok(NULL, " \n");
            i++;
        }
        args[i] = NULL; // Ensure the last element is NULL

        // Print the first argument to debug
        if (debug)
            printf("First argument: %s\n", args[0]);

        /*
        * Ignore blank lines and comments, which begin with the # character.
        */
        if (args[0] == NULL || args[0][0] == '#') { }

        /*
        * Built-in command exit
        */
        else if (strcmp(args[0], "exit") == 0) 
        {
            exit_shell();
        }

        /*
        * Built-in command cd
        */
        else if (strcmp(args[0], "cd") == 0)
        {
            cd_shell(args);
        } 

        /*
        Built-in command pwd
        */
        else if (strcmp(args[0], "pwd") == 0)
        {
            pwd_shell();
        }

        /*
        * Built-in command status
        */
        else if (strcmp(args[0], "status") == 0)
        {
            status_shell(args);
        } 
        
        /*
        * Non built-in command
        * Only runs if not in foreground-only mode
        */
        else
        {
            if (!foreground_only_mode)
            {
                execute_command(args);
            }
        }
    }

    return EXIT_SUCCESS;
}
