#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include "LineParser.h"
#include <sys/types.h>

#define BUFFER_SIZE 2048

//int execute(cmdLine *pCmdLine);

int execute(cmdLine *pCmdLine) {
    pid_t pid;

    pid = fork();
    if (pid == -1) {
        perror("fork() error");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {// changed the execv to execvp according to t0a 
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("execvp() error");
            _exit(EXIT_FAILURE);
        }
    } else {
        waitpid(pid, NULL, 0);
    }

    return 0;
}

int main(int argc, char **argv) {
    char buffer[BUFFER_SIZE];
    char cwd[PATH_MAX];
    cmdLine *parsedLine;

    while (1) {
        if (getcwd(cwd, PATH_MAX) == NULL) {
            perror("getcwd() error");
            exit(EXIT_FAILURE);
        }

        printf("%s> ", cwd);
        fflush(stdout);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            perror("fgets() error");
            exit(EXIT_FAILURE);
        }

        parsedLine = parseCmdLines(buffer);

        if (strcmp(parsedLine->arguments[0], "quit") == 0) {
            freeCmdLines(parsedLine);
            exit(EXIT_SUCCESS);
        }

        execute(parsedLine);
        freeCmdLines(parsedLine);
    }

    return 0;
}


