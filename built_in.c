/* 
* Program file: built_in.c
* Author: Stef Timmermans
* Date: 26 May 2023
* Description:
*   This file implements the built-in commands for
*   the smallsh program. These are:
*
*     exit
*     cd [dir]
*     status
*
*/

#include "built_in.h"

void exit_shell() 
{
    // Kill processes in process array
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i] != 0)
            kill(processes[i], SIGKILL);
    }

    // Then...
    exit(0);
}

void cd_shell(char* args[512])
{
    if (args[1] == NULL)
    {
        /*
        * Change to the HOME directory by default
        */

        char* home = getenv("HOME");
        if (chdir(home) != 0) // Call to change directory
            perror("chdir");
    }
    else
    {
        /*
        * Argument provided, change to that directory
        */

        if (chdir(args[1]) != 0) // Call to change directory
            perror("chdir(...) in cd_shell failed!");
    }
}

void pwd_shell()
{
    // Print out the current working directory to stdout
    char cwd[2048];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("getcwd(...) in pwd_shell failed!");
}

void status_shell(char* args[512])
{
    // Use global variable in `main.h` to print out the status
    printf("exit value %d\n", last_exit_status);
}
