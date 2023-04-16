#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <sys/wait.h>


#define MAX_INPUT_SIZE 2048

int debugMode = 0;

void printPrompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, PATH_MAX) != NULL) {
        printf("%s$ ", cwd);
        fflush(stdout);
    } else {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }
}

void printDebug(char *msg) {
    if (debugMode) {
        fprintf(stderr, "DEBUG: %s\n", msg);
    }
}

void execute(cmdLine *pCmdLine) {
    int res = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
    if (res == -1) {
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    int quit = 0;
    char input[MAX_INPUT_SIZE];
    cmdLine *pCmdLine;

    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debugMode = 1;
    }

    while (!quit) {
        printPrompt();
        fgets(input, MAX_INPUT_SIZE, stdin);
        pCmdLine = parseCmdLines(input);
        printDebug("Command parsed");

        if (strcmp(pCmdLine->arguments[0], "quit") == 0) {
            quit = 1;
        } else {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork failed");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                printDebug("Child process created");
                execute(pCmdLine);
            } else {
                int status;
                waitpid(pid, &status, 0);
                printDebug("Child process terminated");
            }
        }

        freeCmdLines(pCmdLine);
    }

    printf("Exiting...\n");
    return 0;
}
/*
gcc -g -Wall -m32 -c -o GPT.o GPT.c
gcc -m32 -g -Wall -o GPT GPT.o
*/