

// #### SIMONS ###

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include "LineParser.h"
#include <signal.h>
// #include <errno.h>


/*
// ~~~~~~~~~~~~ task 3 stuff: ~~~~~~~~~~~~`
*/
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0


typedef struct process {
    cmdLine* cmd;
    pid_t pid;
    int status;
    struct process* next;
} process;

void printProcessList(process** process_list);

void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
    printf("\t~~DEBUG:~~ at AddProccess: cmd is: %s\n",cmd->arguments[0]); //DEBUG!!!
    process* new_process = (process*)malloc(sizeof(process));
    if(*process_list == NULL){  //if list is empty
        *process_list = new_process;
    } else {
        process* curr = *process_list;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = new_process;
    }
    new_process->cmd = cmd;
    new_process->pid = pid;
    new_process->status = RUNNING;
    new_process->next = NULL;
    printProcessList(process_list); //added for debug

}


void printProcessList(process** process_list) {
    printf("PID\tCommand\t\t\tSTATUS\n");
    printf("---\t-------\t\t\t------\n");
    process* curr = *process_list;
    // int i = 1;
    while (curr != NULL) {
        printf("%d\t%s\t\t", curr->pid, curr->cmd->arguments[0]);
        if (curr->status == TERMINATED) {
            printf("Terminated\n");
        } else if (curr->status == RUNNING) {
            printf("Running\n");
        } else {
            printf("Suspended\n");
        }
        curr = curr->next;
        // i++;
    }
}

void freeProcessList(process** process_list) {      //like in the virusList
    printf("\t~~DEBUG:~~ at FreeProccessList\n"); //DEBUG!!!

    process* curr = *process_list;
    while (curr != NULL) {
        process* temp = curr;
        curr = curr->next;
        freeCmdLines(temp->cmd);
        free(temp);
    }
    *process_list = NULL;
}


process* process_list = NULL;   //global variable


/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
    
int execute(cmdLine *pCmdLine) {
    printf("\t~~DEBUG:~~ cmd is: %s\n",pCmdLine->arguments[0]); //DEBUG!!!
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


        //for task 3 - use PROCS:
        if (strcmp(parsedCmdLine->arguments[0], "procs") == 0) {
            printProcessList(&process_list);

        // int pid = atoi(parsedCmdLine->arguments[1]);
        // if (kill(pid, SIGTSTP) == -1) {
        //     perror("Failed to suspend process");
        //     }
        //     // freeCmdLines(parsedCmdLine);
            continue;
        }
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






        //$$$$$$$$$$$$$ CONTINIUE FROM HERE! $$$$$$$$$$$$$$$$$






        else {
        parsedCmdLine = parseCmdLines(input);
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
                        addProcess(&process_list, parsedCmdLine, pid);
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
           
    //~~~~~~~~~~~~~~~~~~~~~ref - mypipeline*~~~~~~~~~~~~~~~~~~~`
            int pipefd[2];
            pid_t child1_pid, child2_pid;

            if (pipe(pipefd) == -1) {
                perror("pipe ERROR");
                exit(EXIT_FAILURE);
            }

            // ~~CHILD_1~~~:
            // fprintf(stderr, "(parent_process>forking…)\n");
            child1_pid = fork();
            // if (child1_pid!=0 ) fprintf(stderr, "(parent_process>created process with id: %d)\n",child1_pid);
            if (child1_pid == -1) {
                perror("fork ERROR");
                exit(EXIT_FAILURE);
            }
                

            freeCmdLines(parsedCmdLine);
            parsedCmdLine=parseCmdLines(cmd1);
            if (child1_pid == 0) {

                
               
                // fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
                close(STDOUT_FILENO);
                dup(pipefd[1]);
                close(pipefd[1]);
                if (parsedCmdLine->outputRedirect==NULL){
                    // fprintf(stderr, "(child1>going to execute cmd: %s)\n",cmd1);
                    int execReturnVal = execute(parsedCmdLine);
                    if (execReturnVal == -1) {
                        _exit(EXIT_FAILURE);
                    }
                } else fprintf(stderr, "output redirection makes no sense in cmd: %s)\n",cmd1);

                exit(0);
                
            } else {
            // ~~PARENT~~~:
                if (debugMode) {
                    printDebugInfo(child1_pid, parsedCmdLine->arguments[0]);
                }

                close(pipefd[1]);
                // addProcess(&process_list, parsedCmdLine, child1_pid);

                // fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
            }

            // ~~CHILD_2~~~:
            // fprintf(stderr, "(parent_process>forking…)\n"); 
            child2_pid = fork();
            // if (child2_pid!=0 ) fprintf(stderr, "(parent_process>created process with id: %d)\n",child2_pid);
            if (child2_pid == -1) {
                perror("fork ERROR");
                exit(EXIT_FAILURE);
            }
            

            freeCmdLines(parsedCmdLine);
            parsedCmdLine=parseCmdLines(cmd2);
            if (child2_pid == 0) {
                // fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
                close(STDIN_FILENO);
                dup(pipefd[0]);
                close(pipefd[0]);

                if (parsedCmdLine->inputRedirect==NULL){

                    // fprintf(stderr, "(child1>going to execute cmd: %s)\n",cmd2);
                    int execReturnVal = execute(parsedCmdLine);
                    if (execReturnVal == -1) {
                        _exit(EXIT_FAILURE);
                    }
                } else fprintf(stderr, "output redirection makes no sense in cmd: %s)\n",cmd1);

                exit(0);
            } else {
            // ~~PARENT~~~:
                if (debugMode) {
                    printDebugInfo(child2_pid, parsedCmdLine->arguments[0]);
                }
                close(pipefd[0]);
                addProcess(&process_list, parsedCmdLine, child2_pid);

                // fprintf(stderr, "(parent_process>closing the read end of the pipe...)\n");
            }

            // fprintf(stderr, "(parent_process>waiting for child processes to terminate...)\n");
            waitpid(child1_pid, NULL, 0);
            waitpid(child2_pid, NULL, 0);

            // fprintf(stderr, "(parent_process>exiting...)\n");

            }
        }
            freeCmdLines(parsedCmdLine);
    }
    freeProcessList(&process_list);
}
