#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/wait.h>
#include "LineParser.h"

// char *get_working_dir() {
//     char *buffer = malloc(PATH_MAX);
//     if (buffer == NULL) {
//         perror("malloc");
//         exit(EXIT_FAILURE);
//     }
//     if (getcwd(buffer, PATH_MAX) == NULL) {
//         perror("getcwd");
//         exit(EXIT_FAILURE);
//     }
//     return buffer;
// }



/* DEBUG:*/
void printDebug(char *msg,int isDebug,int pid, char* command) {
    if (isDebug) {
        fprintf(stderr, "DEBUG: %s,pid is:%i, command is: %s \n", msg,pid,command);
    }
}

void execute(cmdLine *pCmdLine) {
    int isDebug = 0;
    for(int i=0;i<pCmdLine->argCount  ; i++ )
    {
        if (strcmp(pCmdLine->arguments[i], "-d") == 0) {
            isDebug = 1;
        }
    }
    int status;
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // child process
        printDebug("Child process created",isDebug,pid,pCmdLine->arguments[0]);
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {    //changed: execv->execvp
            perror("execv failed");
            _exit(EXIT_FAILURE);        //exit abnormally, no cleaning nor atExit stuff
        }
    } else { // parent process
        // if (pCmdLine->blocking=='1')
         waitpid(pid, &status, 0);
        printDebug("Child process terminated",isDebug,pid,"none");
    }
}




int main(int argc, char **argv) {
    int maxInputSize = 2048;
    char input[maxInputSize];
    cmdLine *parsed_cmd;
    char *working_dir;
    while (1) {
        
        char *buffer = malloc(PATH_MAX);
        working_dir = getcwd(buffer, PATH_MAX);
        printf("%s: ", working_dir);

        if (fgets(input, maxInputSize, stdin) == NULL) {
            perror("fgets Error");
            exit(EXIT_FAILURE);
        }

        parsed_cmd = parseCmdLines(input);
        if (parsed_cmd == NULL) {
            printf("Invalid command\n");
            continue;
        }

        if (strcmp(parsed_cmd->arguments[0], "quit") == 0) {
            freeCmdLines(parsed_cmd);
            printf("Exit shell normally\n");
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(parsed_cmd->arguments[0], "cd") == 0) {
            if (chdir(parsed_cmd->arguments[1]) == -1) {
                fprintf(stderr, "cd ERROR: couldn't open directory %s\n", parsed_cmd->arguments[1]);
            }
        }

    // printf("check 1 2 3\n");
    int isDebug = 0;
    for(int i=0;i<parsed_cmd->argCount  ; i++ )
    {
        if (strcmp(parsed_cmd->arguments[i], "-d") == 0) {
            isDebug = 1;
        }
    }
    int status;
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // child process
        printDebug("Child process created",isDebug,pid,parsed_cmd->arguments[0]);
        if (execvp(parsed_cmd->arguments[0], parsed_cmd->arguments) == -1) {    //changed: execv->execvp
            perror("execv failed");
            _exit(EXIT_FAILURE);        //exit abnormally, no cleaning nor atExit stuff
        }
    } else { // parent process
        // if (parsed_cmd->blocking=='1')
         waitpid(pid, &status, 0);
        printDebug("Child process terminated",isDebug,pid,"none");
    }
        execute(parsed_cmd);
        free(working_dir);
        freeCmdLines(parsed_cmd);   //release cmdLine resources
    }

    return 0;
    }

