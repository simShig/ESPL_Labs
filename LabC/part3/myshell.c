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
// #define HISTLEN 20


typedef struct process{
    cmdLine* cmd; /* the parsed command line*/
    pid_t pid; /* the process id that is running the command*/
    int status; /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next; /* next process in chain */
} process;



void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    // printf("\t~~DEBUG:~~ at AddProccess: cmd is: %s\n",cmd->arguments[0]); //DEBUG!!!
    process* newProcess = malloc(sizeof(struct process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    newProcess->next = NULL;
    newProcess->status = RUNNING;
    if(cmd->blocking){  // || cmd->next
        newProcess->status = TERMINATED;
    }
    if(*process_list == NULL){
        *process_list = newProcess;
    } else {
        process* curr = *process_list;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = newProcess;
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


// void freeProcessList(process* process_list){
//     if(process_list != NULL){
//         freeProcessList(process_list->next);
//         freeCmdLines(process_list->cmd);
//         free(process_list);
//     }
// }

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
        if(WIFSTOPPED(status)){
            updateProcessStatus(*process_list, pid, SUSPENDED);
        } 
        if(WIFCONTINUED(status)){
            updateProcessStatus(*process_list, pid, RUNNING);
        } 
        if(WIFEXITED(status) ){       // exited 
            printf("\t~~DEBUG!!~~im in ifTerminated - cmd id is: %d \n",pid);
            updateProcessStatus(*process_list, pid, TERMINATED);
        } 
        if(WIFSIGNALED(status) ){       // killed    
            printf("\t~~DEBUG!!~~im in ifTerminated - cmd id is: %d \n",pid);
            updateProcessStatus(*process_list, pid, TERMINATED);
        } 

    }
}

void removeTermiantedProcs(process** process_list){
    process* curr = *process_list;
    process* prev = *process_list;
    while(curr != NULL){
        if(curr->status == TERMINATED){
            if(prev == curr){
                *process_list = (*process_list)->next;
                freeCmdLines(curr->cmd);
                free(curr);
                curr = *process_list;
                prev = *process_list;
            } else {
                prev->next = curr->next;
                freeCmdLines(curr->cmd);
                free(curr);
                curr = prev->next;
            }
        } else {
            prev = curr;
            curr = curr->next;
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

// void printProcessList(process** process_list){
//     updateProcessList(process_list);
//     process* curr = *process_list;
//     printf("PID\tCommand\tSTATUS\n");
//     while(curr != NULL){
//         char* status = "RUNNING";
//         if(curr->status == TERMINATED) status = "TERMINATED";
//         if(curr->status == SUSPENDED) status = "SUSPENDED";
//         printf("%d\t%s\t%s\n", curr->pid, curr->cmd->arguments[0], status);
//         curr = curr->next;
//     }     
//     //after printing - remove the ones terminated  
//     removeTermiantedProcs(process_list);
// }

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



// //receives a parsed line and invokes the program specified in the cmdLine using the proper system call.
// void execute(cmdLine *pCmdLine){
//     if(pCmdLine -> inputRedirect){
//         fclose(stdin);
//         if(fopen(pCmdLine ->inputRedirect,"r") == NULL){
//             perror("Execution FAILED");
//             _exit(1);
//         }
//     }

//     if(pCmdLine -> outputRedirect){
//         fclose(stdout);
//         if(fopen(pCmdLine ->outputRedirect,"w") == NULL){
//             perror("Execution FAILED");
//             _exit(1);
//         }
//     }
//     char *command = (*pCmdLine).arguments[0];
//     if(execvp(command,(*pCmdLine).arguments) == -1){ // executing command + display error if execv failed.
//         perror("Execution FAILED");
//         _exit(1);
//     }
// }

// void cd(cmdLine *pCmdLine){
//     chdir(pCmdLine -> arguments[1]);
// }

// void makePipe(cmdLine *parsedCmdLine){
//     int pipefd[2]; // 0 read 1 write
//     pid_t pid1, pid2;
//     if(parsedCmdLine->outputRedirect || parsedCmdLine->next->inputRedirect){ // redirection
//         fprintf(stderr, "Invalid redirection\n");
//         return;
//     }
//     if (pipe(pipefd) == -1) {
//         perror("Failed to pipe");
//         _exit(-1);
//     }

//     pid1 = fork();
//     if (pid1 == 0) { // child 1 process
//         close(STDOUT_FILENO); // close standard output
//         dup(pipefd[1]); // duplicate write-end of pipe
//         close(pipefd[1]); // close write-end of pipe

//         if(parsedCmdLine -> inputRedirect){ // redirection
//         fclose(stdin);
//             if(fopen(parsedCmdLine ->inputRedirect,"r") == NULL){
//                 perror("Execution FAILED");
//                 _exit(1);
//             }
//         }
//         if(execvp(parsedCmdLine->arguments[0],parsedCmdLine->arguments) == -1){ // executing command + display error if execv failed.
//             perror("Execution FAILED");
//             _exit(1);
//         }
//     }else { // parent process
//         close(pipefd[1]);
//         if(parsedCmdLine->next !=NULL){
//             pid2 = fork();
//             if (pid2 == 0) { // child 2 process
//                 close(STDIN_FILENO);
//                 dup(pipefd[0]);
//                 close(pipefd[0]);

//                 if(parsedCmdLine->next -> outputRedirect){ // redirection
//                     fclose(stdout);
//                     if(fopen(parsedCmdLine->next ->outputRedirect,"w") == NULL){
//                         perror("Execution FAILED");
//                         _exit(1);
//                     }
//                 }
//                 if(execvp(parsedCmdLine->next->arguments[0],parsedCmdLine->next->arguments) == -1){ // executing command + display error if execv failed.
//                     perror("Execution FAILED");
//                     _exit(1);
//                 }

//             } else {
//                 close(pipefd[0]);
//                 addProcess(&process_list, parsedCmdLine, pid1);
//                 waitpid(pid1, NULL, 0);
//                 waitpid(pid2, NULL, 0);
//             }
//         }

//     }
// }


void printDebugInfo(pid_t pid, char* command) {
    fprintf(stderr, "PID: %d\n", pid);
    fprintf(stderr, "Executing command: %s\n", command);
}

void handleSignal(int signal) {
    printf("Received signal: %d\n", signal);
}

//global vars:
struct process* process_list = NULL;
int debug = 0;
int HISTLEN = 20;


//~~~~~~~~~~ main - definitions ~~~~~~~~~~~

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
    int historySize = 0;

    
    // char cwd[PATH_MAX];
    // char input[2048];
    // struct cmdLine *parsedCmdLine;
    // for(int i=0; i<argc; i++){
	// 	if(strcmp(argv[i],"-d") == 0)
	// 		debug = 1;
	// }
//~~~~~~~~~~ main - loop while ~~~~~~~~~~~

    while(1){
        // int shouldFree = 1;
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







        // if(getcwd(cwd, PATH_MAX) == 0) //print cwd
        //     return 1;
        // printf("%s$ ", cwd);

        // fgets(input,2048,stdin);


        // if(strncmp("quit",input,4) == 0){ //end inf loop with "quit"
        //     freeProcessList(process_list);
        //     exit(0);
        // }

        //~~~~~~~~~~` HISTORY stuff - start ~~~~~~~~~~~~

        if(strcmp(parsedCmdLine -> arguments[0],"history") ==0){
            for(int i = 0; i < historySize; i++){
                printf("%d) %s", i + 1, history[(oldest + i) % HISTLEN]);
            }
            freeCmdLines(parsedCmdLine);        //i cleaned freeCmdLines!
            continue;
        }
        else if(strcmp(parsedCmdLine -> arguments[0],"!!") ==0){
            freeCmdLines(parsedCmdLine);
            if(historySize < 1){
                fprintf(stderr, "History is empty!\n");
                continue;
            } else {
                parsedCmdLine = parseCmdLines(history[(newest-1 + HISTLEN) % HISTLEN]);
            }
        }
        else if(parsedCmdLine -> arguments[0][0] == '!'){
            int index = atoi(parsedCmdLine -> arguments[0]+1) - 1;
            freeCmdLines(parsedCmdLine);
            if(index >= historySize || index < 0){
                fprintf(stderr, "index %d is out of bound %d!\n", index, historySize);
                continue;
            } else {
                parsedCmdLine = parseCmdLines(history[(oldest + index + HISTLEN) % HISTLEN]);
            }
        }
        else {
            strcpy(history[newest], input);
            newest = (newest + 1) % HISTLEN;
            if(historySize == HISTLEN){
                oldest = (oldest + 1) % HISTLEN;
            } else {
                historySize++;
            }
        }

        //~~~~~~~~~~` HISTORY stuff - end ~~~~~~~~~~~~
        
        
        //~~~~~~~~~~` pipe stuff - start ~~~~~~~~~~~~
        // if(parsedCmdLine->next !=NULL){
        //     makePipe(parsedCmdLine);
        //     shouldFree = 0;
        // }
        //~~~~~~~~~~` pipe stuff - end ~~~~~~~~~~~~

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

        if (strcmp(parsedCmdLine->arguments[0], "procs") == 0) {
            printProcessList(&process_list);

        // int pid = atoi(parsedCmdLine->arguments[1]);
        // if (kill(pid, SIGTSTP) == -1) {
        //     perror("Failed to suspend process");
        //     }
        //     // freeCmdLines(parsedCmdLine);
            continue;
        }
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



        // else if(strcmp(parsedCmdLine -> arguments[0],"cd") ==0){
        //     cd(parsedCmdLine);        }
        // else if(strcmp(parsedCmdLine -> arguments[0],"wake") ==0){
        //     kill(atoi(parsedCmdLine -> arguments[1]),SIGCONT);
        // }
        // else if(strcmp(parsedCmdLine -> arguments[0],"suspend") ==0){
        //     kill(atoi(parsedCmdLine -> arguments[1]),SIGTSTP);
        // }
        // else if(strcmp(parsedCmdLine -> arguments[0],"kill") ==0) {
        //     kill(atoi(parsedCmdLine -> arguments[1]),SIGINT);
        // } else if(strcmp(parsedCmdLine -> arguments[0],"procs") ==0) {
        //     printProcessList(&process_list);
        // }





//$$$$$$$$$$$$$ CONTINIUE FROM HERE! $$$$$$$$$$$$$$$$$


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
            freeCmdLines(parsedCmdLine);
        }
    
    freeProcessList(&process_list);
}






//         else {
//             //we create child process before execute
//             shouldFree = 0;
//             int pid = fork();
//             if(pid != 0){// this is a parent
//                 if((*parsedCmdLine).blocking != 0){ // if no &, we wait for child
//                     waitpid(pid, NULL, 0);
//                 }
//                 addProcess(&process_list, parsedCmdLine, pid);
//             }else{ // this is a child
//                 execute(parsedCmdLine); // DELETED CALL TO EXECUTE
//             }
//             if(debug == 1){
//                 fprintf(stderr,"PID: %d\n",pid);
//                 fprintf(stderr,"Executing command: %s\n",(*parsedCmdLine).arguments[0]);
//             }
//         }
//         if(shouldFree)
//             freeCmdLines(parsedCmdLine);
//     }
//     return 0;
// }