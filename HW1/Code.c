#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define HOST_NAME_MAX 64            // The default value of maximum size of host name string in library <limits.h>.
#define LOGIN_NAME_MAX 256          // The default value of maximum size of login name string in library <limits.h>.
#define MAX_NUMBER_OF_ARGUMENTS 100 // Maximum number of arguments.
#define BUFFER_LEN 1024             // Maximum length of a command with its parameters. 
#define MAX_CURRENT_PATH 100        // The value of maximum size of current path.
#define MAX_PROG_PATH 20            // The value of maximum size of a command path.
#define __BIN_PATH__ "/bin/"        // The path where programs located in.


void executeCD(char** argumentsArr, int numOfArg) {
// "cd" command is not executable command but a built-in the shell.
// So it must be done by chdir() function in library <unistd.h>.
// This function takes the arguments and number of arguments and
// changes the directory. If number of arguments is not eqaul to 2
// or the given parameter is invalid then prints error message.

    if (numOfArg == 2) {
        if (chdir(argumentsArr[1]) == -1) {
            printf("Error: \"%s\" cannot be found\n", argumentsArr[1]);
        }
    }
    else if (numOfArg == 1)
        printf("Error: Parameter must be given.\n");
    else
        printf("Error: Cannot change the library.\n");
}

int main() {
    int i;                                          // Used only for iterations.
    int numOfArg;                                   // Number of arguments in the command.
    int forkReturn;                                 // Return value of fork()
    char hostname[HOST_NAME_MAX];                   // Computer's hostname.
    char username[LOGIN_NAME_MAX];                  // Computer's username.
    char line[BUFFER_LEN];                          // Commands which entered by the user.
    char progpath[MAX_PROG_PATH];                   // Path of the program which will be executed.
    char currentPath[MAX_CURRENT_PATH];             // Current path of the shell program.
    char *argumentsArr[MAX_NUMBER_OF_ARGUMENTS];    // Array of argument pointers.
    char *spaceToken;                               // Each token splited by space character.
    size_t lineLength;                              // Size of the command.

    gethostname(hostname, HOST_NAME_MAX);
    cuserid(username);

    while (1) {
        getcwd(currentPath, sizeof(currentPath));
        printf("%s@%s:%s$ ", username, hostname, currentPath); // Print the head of command.

        if (!fgets(line, BUFFER_LEN, stdin)) {
            printf("exit\n");                // If user press CTRL + D then exit.
            return -1;
        }
        lineLength = strlen(line);
        if (line[lineLength - 1] == '\n')
            line[lineLength - 1] = '\0';

        if (!strcmp(line, "exit")) {        // Check if command is "exit".
            return -2;
        }

        spaceToken = strtok(line, " ");

        for (numOfArg = 0; spaceToken != NULL; numOfArg++) {
            argumentsArr[numOfArg] = spaceToken;
            spaceToken = strtok(NULL, " ");
        }
        argumentsArr[numOfArg] = NULL;

        strcpy(progpath, __BIN_PATH__);       // Copy to the file path
        strcat(progpath, argumentsArr[0]);    // Add program to path

        for (i = 0; i < strlen(progpath); i++) { // Delete all new lines.
            if (progpath[i] == '\n') {
                progpath[i] = '\0';
            }
        }

        if (!strcmp(argumentsArr[0], "cd"))  // if command is "cd", don't fork.
            executeCD(argumentsArr, numOfArg);

        else {
            forkReturn = fork();             // Fork operation is done.
            if (forkReturn == 0) {           // Child process.
                if (numOfArg == 1) {
                    execl(progpath, progpath, NULL , NULL);
                }
                else
                    execvp(progpath , argumentsArr);
                fprintf(stderr, "Error: Child process could not do execvp\n");

            } else {                         // Parent process.
                wait(NULL);                  // Wait until child process exits.

            }
        }

    }
}
