#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include "LineParser.h"

#define MAX_INPUT_SIZE 2048

int execute(cmdLine *pCmdLine) {
    int execReturnVal = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    if (execReturnVal == -1) {
        perror("execvp failed");
        return -1;
    }
    return 0;
}

void printDebugInfo(pid_t pid, char* command) {
    fprintf(stderr, "PID: %d\n", pid);
    fprintf(stderr, "Executing command: %s\n", command);
}

int main(int argc, char **argv) {
    int debugMode = 0;
    char input[MAX_INPUT_SIZE];
    cmdLine *parsedCmdLine;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debugMode = 1;
        }
    }

    while (1) {
        getcwd(input, MAX_INPUT_SIZE);
        printf("%s$ ", input);
        fgets(input, MAX_INPUT_SIZE, stdin);
        parsedCmdLine = parseCmdLines(input);

        if (strcmp(parsedCmdLine->arguments[0], "quit") == 0) {
            freeCmdLines(parsedCmdLine);
            exit(0);
        }
/////////////////i added this i order to implemet task 1 c:
        if (strcmp(parsedCmdLine->arguments[0], "cd") == 0) {
            if (chdir(parsedCmdLine->arguments[1]) == -1) {
                fprintf(stderr, "cd: %s: No such file or directory\n", parsedCmdLine->arguments[1]);
            }
            freeCmdLines(parsedCmdLine);
            continue;
        }
        ////////////////////////
        pid_t pid = fork();

        if (pid == 0) { // child process
            int execReturnVal = execute(parsedCmdLine);
            if (execReturnVal == -1) {
                _exit(EXIT_FAILURE);
            }
        } else if (pid > 0) { // parent process
            if (debugMode) {
                printDebugInfo(pid, parsedCmdLine->arguments[0]);
            }
            freeCmdLines(parsedCmdLine);
            if (parsedCmdLine->blocking) {
                waitpid(pid, NULL, 0);
            }
        } else { // fork failed
            perror("fork failed");
            freeCmdLines(parsedCmdLine);
            exit(EXIT_FAILURE);
        }
    }
}
