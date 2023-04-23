#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include "LineParser.h"
#include <signal.h>
#include <errno.h>

    
int execute(cmdLine *pCmdLine) {
    int execReturnVal;

    if (pCmdLine->inputRedirect != NULL) {
        FILE *inputFile = fopen(pCmdLine->inputRedirect, "r");
        if (inputFile == NULL) {
            perror("Error opening input file");
            return -1;
        }

        dup2(fileno(inputFile), STDIN_FILENO);  //redirection stdin2file - referance from stackOverflow - https://stackoverflow.com/questions/14543443/in-c-how-do-you-redirect-stdin-stdout-stderr-to-files-when-making-an-execvp-or
        fclose(inputFile);
    }

    if (pCmdLine->outputRedirect != NULL) {
        FILE *outputFile = fopen(pCmdLine->outputRedirect, "w");
        if (outputFile == NULL) {
            perror("Error opening output file");
            return -1;
        }
        dup2(fileno(outputFile), STDOUT_FILENO); //redirection file2stdin - referance from stackOverflow - https://stackoverflow.com/questions/14543443/in-c-how-do-you-redirect-stdin-stdout-stderr-to-files-when-making-an-execvp-or
        fclose(outputFile);
    }

    execReturnVal = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
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

void handleSignal(int signal) {
    printf("Received signal: %d\n", signal);
}

int main(int argc, char **argv) {
    int maxInputSize = 2048;
    int debugMode = 0;
    char input[maxInputSize];
    cmdLine *parsedCmdLine;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debugMode = 1;
        }
    }

    while (1) {
        getcwd(input, maxInputSize);
        printf("%s$>>: ", input);
        fgets(input, maxInputSize, stdin);
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
        //added this in order to implement 3:
        signal(SIGTSTP, handleSignal);
        signal(SIGCONT, handleSignal);
        signal(SIGINT, handleSignal);
        ////////////////////////
        pid_t pid = fork();

        //added this in order to use suspend wake and kill  
        if (strcmp(parsedCmdLine->arguments[0], "suspend") == 0) {
        int pid = atoi(parsedCmdLine->arguments[1]);
        if (kill(pid, SIGTSTP) == -1) {
            perror("Failed to suspend process");
            }
        freeCmdLines(parsedCmdLine);
        continue;
        } else if (strcmp(parsedCmdLine->arguments[0], "wake") == 0) {
            int pid = atoi(parsedCmdLine->arguments[1]);
            if (kill(pid, SIGCONT) == -1) {
            perror("Failed to wake up process");
        }
        freeCmdLines(parsedCmdLine);
        continue;
        } else if (strcmp(parsedCmdLine->arguments[0], "kill") == 0) {
            int pid = atoi(parsedCmdLine->arguments[1]);
            if (kill(pid, SIGKILL) == -1) {
            perror("Failed to kill process");
        }
        freeCmdLines(parsedCmdLine);
        continue;
        }
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