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
        if (*input==10){
             printf("\t\t im here DEBUG!\n");
             continue;
        }
        parsedCmdLine = parseCmdLines(input);

        if (strcmp(parsedCmdLine->arguments[0], "quit") == 0) {
            freeCmdLines(parsedCmdLine);
            exit(0);
        }
        if (strcmp(parsedCmdLine->arguments[0], "cd") == 0) {
            if (chdir(parsedCmdLine->arguments[1]) == -1) {
                fprintf(stderr, "cd: %s: No such file or directory\n", parsedCmdLine->arguments[1]);
            }
            // freeCmdLines(parsedCmdLine);
            continue;
        }

        signal(SIGTSTP, handleSignal);
        signal(SIGCONT, handleSignal);
        signal(SIGINT, handleSignal);


        //added this in order to use suspend wake and kill  
        if (strcmp(parsedCmdLine->arguments[0], "suspend") == 0) {
        int pid = atoi(parsedCmdLine->arguments[1]);
        if (kill(pid, SIGTSTP) == -1) {
            perror("Failed to suspend process");
            }
            // freeCmdLines(parsedCmdLine);
        continue;
        } else if (strcmp(parsedCmdLine->arguments[0], "wake") == 0) {
            int pid = atoi(parsedCmdLine->arguments[1]);
            if (kill(pid, SIGCONT) == -1) {
            perror("Failed to wake up process");
        }
            // freeCmdLines(parsedCmdLine);
        continue;
        } else if (strcmp(parsedCmdLine->arguments[0], "kill") == 0) {
            int pid = atoi(parsedCmdLine->arguments[1]);
            if (kill(pid, SIGKILL) == -1) {
            perror("Failed to kill process");
        }
            // freeCmdLines(parsedCmdLine);
        continue;
        }
        else {

        char* cmd1 = strtok(input, "|");
        char* cmd2 = strtok(NULL, "|");
        if (cmd2 == NULL) {     //case its "single" command
                    pid_t pid = fork();
                    if (pid == 0) { // child process
                        int execReturnVal = execute(parsedCmdLine);
                        if (execReturnVal == -1) {
                            _exit(EXIT_FAILURE);
                        }
                        exit(0);
                    } else if (pid > 0) { // parent process
                        if (debugMode) {
                            printDebugInfo(pid, parsedCmdLine->arguments[0]);
                        }
                        // freeCmdLines(parsedCmdLine);
                        if (parsedCmdLine->blocking) {
                            waitpid(pid, NULL, 0);
                        }
                    } else { // fork failed
                        perror("fork failed");
                        freeCmdLines(parsedCmdLine);
                        exit(EXIT_FAILURE);
                    }        
        } else {        //case its "dual" command:
           
    //~~~~~~~~~~~~~~~~~~~~~mypipeline*~~~~~~~~~~~~~~~~~~~`
            int pipefd[2];
            pid_t child1_pid, child2_pid;

            // Step 1: create a pipe
            if (pipe(pipefd) == -1) {
                perror("pipe ERROR");
                exit(EXIT_FAILURE);
            }

            // Step 2: fork the first child process (child1)
            fprintf(stderr, "(parent_process>forking…)\n");
            child1_pid = fork();
            if (child1_pid!=0 ) fprintf(stderr, "(parent_process>created process with id: %d)\n",child1_pid);
            if (child1_pid == -1) {
                perror("fork ERROR");
                exit(EXIT_FAILURE);
            }

            if (child1_pid == 0) {

                
               
                // Step 3: in child1 process
                fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
                // close the standard output
                close(STDOUT_FILENO);
                // duplicate the write-end of the pipe
                dup(pipefd[1]);
                // close the file descriptor that was duplicated
                close(pipefd[1]);
                // execute cmd1
                freeCmdLines(parsedCmdLine);
                parsedCmdLine=parseCmdLines(cmd1);
                if (parsedCmdLine->outputRedirect==NULL){
                    printf ("\t@@DEBUG1@@\n");         //DEBUG~~~~
                    fprintf(stderr, "(child1>going to execute cmd: %s)\n",cmd1);
                    int execReturnVal = execute(parsedCmdLine);
                    if (execReturnVal == -1) {
                        _exit(EXIT_FAILURE);
                    }
                } else fprintf(stderr, "output redirection makes no sense in cmd: %s)\n",cmd1);

                exit(0);
                
            } else {
                if (debugMode) {
                    printDebugInfo(child1_pid, parsedCmdLine->arguments[0]);
                }
                // Step 4: in parent process
                // close the write end of the pipe
                close(pipefd[1]);
                fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
            }
            // freeCmdLines(parsedCmdLine);

            // Step 5: fork the second child process (child2)
            fprintf(stderr, "(parent_process>forking…)\n"); 
            child2_pid = fork();
            if (child2_pid!=0 ) fprintf(stderr, "(parent_process>created process with id: %d)\n",child2_pid);
            if (child2_pid == -1) {
                perror("fork ERROR");
                exit(EXIT_FAILURE);
            }

            if (child2_pid == 0) {
                // Step 6: in child2 process
                fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
                // close the standard input
                close(STDIN_FILENO);
                // duplicate the read-end of the pipe
                dup(pipefd[0]);
                // close the file descriptor that was duplicated
                close(pipefd[0]);
                // execute cmd2

                freeCmdLines(parsedCmdLine);
                parsedCmdLine=parseCmdLines(cmd2);
                if (parsedCmdLine->inputRedirect==NULL){
                    printf ("\t@@DEBUG2@@\n");          //DEBUG~~~~

                    fprintf(stderr, "(child1>going to execute cmd: %s)\n",cmd2);
                    int execReturnVal = execute(parsedCmdLine);
                    if (execReturnVal == -1) {
                        _exit(EXIT_FAILURE);
                    }
                } else fprintf(stderr, "output redirection makes no sense in cmd: %s)\n",cmd1);

                exit(0);
            } else {
                if (debugMode) {
                    printDebugInfo(child2_pid, parsedCmdLine->arguments[0]);
                }
                // Step 7: in parent process
                // close the read end of the pipe
                close(pipefd[0]);
                fprintf(stderr, "(parent_process>closing the read end of the pipe...)\n");
            }

            // Step 8: wait for child processes to terminate
            fprintf(stderr, "(parent_process>waiting for child processes to terminate...)\n");
            waitpid(child1_pid, NULL, 0);
            waitpid(child2_pid, NULL, 0);

            // before exiting
            fprintf(stderr, "(parent_process>exiting...)\n");





            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // if (pid == 0) { // child process
            //     int execReturnVal = execute(parsedCmdLine);
            //     if (execReturnVal == -1) {
            //         _exit(EXIT_FAILURE);
            //     }
            //     exit(0);
            // } else if (pid > 0) { // parent process
            //     if (debugMode) {
            //         printDebugInfo(pid, parsedCmdLine->arguments[0]);
            //     }
            //     freeCmdLines(parsedCmdLine);
            //     if (parsedCmdLine->blocking) {
            //         waitpid(pid, NULL, 0);
            //     }
            // } else { // fork failed
            //     perror("fork failed");
            //     freeCmdLines(parsedCmdLine);
            //     exit(EXIT_FAILURE);
            // }
            }
        }
            freeCmdLines(parsedCmdLine);
    }
}
