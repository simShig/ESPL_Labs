#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "linux/limits.h"
#include "LineParser.h"

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

int iprocsCounter=0;       //for freeing

typedef struct process{
    cmdLine* cmd; 
    pid_t pid;
    int status;
    struct process *next;
} process;


void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    iprocsCounter++;
    process* newProcess = malloc(sizeof(struct process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    newProcess->status = RUNNING;
    if(cmd->blocking){  
        newProcess->status = TERMINATED;
    }
    newProcess->next = *process_list; 
    *process_list = newProcess; 
}


void freeProcessList(process* process_list) {
    process* current = process_list;
    process* next;
    
    while (current != NULL) {
        next = current->next;
        freeCmdLines(current->cmd);
        free(current);
        current = next;
    }
}

void updateProcessStatus(process* process_list, int pid, int status){
    process* curr = process_list;
    while(curr!= NULL){
        if(curr->pid == pid){
            curr->status = status;
            return;
        }
        curr = curr->next;
    }
}


void updateProcessList(process **process_list){     //ref - example on https://linux.die.net/man/2/waitpid
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED )) > 0){      //from waitflags.h: 
        if(WIFSTOPPED(status)){     //suspended
            updateProcessStatus(*process_list, pid, SUSPENDED);
        } 
        if(WIFCONTINUED(status)){       //waken
            // printf("\t~~DEBUG!!~~im in ifTerminated /ifcontinued - cmd id is: %d \n",pid);
            updateProcessStatus(*process_list, pid, RUNNING);
        } 
        if(WIFEXITED(status) ){       // exited 
            // printf("\t~~DEBUG!!~~im in ifTerminated - cmd id is: %d \n",pid);
            updateProcessStatus(*process_list, pid, TERMINATED);
        } 
        if(WIFSIGNALED(status) ){       // killed    
            updateProcessStatus(*process_list, pid, TERMINATED);
        } 

    }
}




void removeTermiantedProcs(process** process_list){
    process* curr = *process_list;
    process* prev = *process_list;
    while(curr != NULL){
        if(curr->status != TERMINATED){
            prev = curr;
            curr = curr->next;
        } else {
            if(prev != curr || curr->cmd==NULL){
                prev->next = curr->next;
                freeCmdLines(curr->cmd);
                free(curr);
                curr = prev->next;
            } else {
                *process_list = (*process_list)->next;
                freeCmdLines(curr->cmd);
                free(curr);
                curr = *process_list;
                prev = *process_list;
            }
        }
    }
}


void printProcessList(process** process_list) {
    printf("PID\tCommand\t\t\tSTATUS\n");
    printf("---\t-------\t\t\t------\n");
    updateProcessList(process_list);
    process* curr = *process_list;
    while (curr != NULL) {
        printf("%d\t%s\t\t\t", curr->pid, curr->cmd->arguments[0]);
        if (curr->status == TERMINATED) {
            printf("Terminated\n");
        } else if (curr->status == RUNNING) {
            printf("Running\n");
        } else {
            printf("Suspended\n");
        }
        curr = curr->next;
    }
        //after printing - remove the ones terminated  
    removeTermiantedProcs(process_list);
}



int execute(cmdLine *pCmdLine) {
    // printf("\t~~DEBUG:~~ cmd is: %s\n",pCmdLine->arguments[0]); //DEBUG!!!
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

//global vars:
struct process* process_list = NULL;
int HISTLEN = 20;


void atExitStuff(){
    freeProcessList(process_list);
}

//~~~~~~~~~~ main  ~~~~~~~~~~~

int main(int argc, char *argv[]){
    int maxInputSize = 2048;
    int debugMode = 0;
    char input[maxInputSize];
    cmdLine *parsedCmdLine;
    //~~~isDebug?
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debugMode = 1;
        }
    }
    char history[HISTLEN][maxInputSize];
    int newest = 0;
    int oldest = 0;
    int currHistLen = 0;

    
//~~~~~~~~~~ main - loop while ~~~~~~~~~~~

    while(1){
        getcwd(input, maxInputSize);
        printf("%s$>>: ", input);
        fgets(input, maxInputSize, stdin);
        if (*input==10){
             continue;
        }
        parsedCmdLine = parseCmdLines(input);


        if (strcmp(parsedCmdLine->arguments[0], "quit") == 0) {
            freeCmdLines(parsedCmdLine);
            if(process_list) freeProcessList(process_list);
            exit(0);
        }
        //~~~~~~~~~~` HISTORY stuff - start ~~~~~~~~~~~~

        if(strcmp(parsedCmdLine -> arguments[0],"history") ==0){
            for(int i = 0; i < currHistLen; i++){
                printf("[%d] %s", i + 1, history[(oldest + i) % HISTLEN]);
            }
            freeCmdLines(parsedCmdLine);        //i cleaned freeCmdLines!
            continue;
        }
        else if(strcmp(parsedCmdLine -> arguments[0],"!!") ==0){
            freeCmdLines(parsedCmdLine);
            if(currHistLen >0){
                parsedCmdLine = parseCmdLines(history[(newest-1 + HISTLEN) % HISTLEN]);
            } else {
                fprintf(stderr, "No history yet!\n");
                continue;
            }
        }
        else if(parsedCmdLine -> arguments[0][0] == '!'){
            int index = atoi(parsedCmdLine -> arguments[0]+1) - 1;
            freeCmdLines(parsedCmdLine);
            
            if(0 < index && index <currHistLen){
                parsedCmdLine = parseCmdLines(history[(oldest + index + HISTLEN) % HISTLEN]);
            } else {
                fprintf(stderr, "index %d out of range (1~%d), no such entry yet!\n", index, currHistLen);
                continue;
            }
        }
        else {
            strcpy(history[newest], input);
            newest = (newest + 1) % HISTLEN;
            if(currHistLen != HISTLEN){
                currHistLen++;
            } else {
                oldest = (oldest + 1) % HISTLEN;
            }
        }

        //~~~~~~~~~~` HISTORY stuff - end ~~~~~~~~~~~~
        
        
        if (strcmp(parsedCmdLine->arguments[0], "cd") == 0) {
            if (chdir(parsedCmdLine->arguments[1]) == -1) {
                fprintf(stderr, "cd: %s: No such file or directory\n", parsedCmdLine->arguments[1]);
            }
            continue;
        }
        signal(SIGTSTP, handleSignal);
        signal(SIGCONT, handleSignal);
        signal(SIGINT, handleSignal);

        if (strcmp(parsedCmdLine->arguments[0], "procs") == 0) {
            printProcessList(&process_list);
            continue;
        }
        if (strcmp(parsedCmdLine->arguments[0], "suspend") == 0) {
        int pid = atoi(parsedCmdLine->arguments[1]);
        if (kill(pid, SIGTSTP) == -1) {
            perror("Failed to suspend process");
            }

        continue;
        } else if (strcmp(parsedCmdLine->arguments[0], "wake") == 0) {
            int pid = atoi(parsedCmdLine->arguments[1]);
            if (kill(pid, SIGCONT) == -1) {
            perror("Failed to wake up process");
        }

        continue;
        } else if (strcmp(parsedCmdLine->arguments[0], "kill") == 0) {
            int pid = atoi(parsedCmdLine->arguments[1]);
            if (kill(pid, SIGKILL) == -1) {
            perror("Failed to kill process");
        }

        continue;
        }

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
        } else {        //case its "dual" command (pipe):

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
            if(!parsedCmdLine) freeCmdLines(parsedCmdLine);
        }
    freeProcessList(process_list);

    if (atexit(atExitStuff) != 0) {
            printf("\n\n nothing to do at exit \n\n");
    }
}


