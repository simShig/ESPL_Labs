#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linux/limits.h"
#include "LineParser.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0
#define HISTLEN 5
typedef struct process{
    cmdLine* cmd; /* the parsed command line*/
    pid_t pid; /* the process id that is running the command*/
    int status; /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next; /* next process in chain */
} process;

struct process* process_list = NULL;
int debug = 0;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process* newProcess = malloc(sizeof(struct process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    newProcess->next = NULL;
    if(cmd->blocking || cmd->next){
        newProcess->status = TERMINATED;
    } else {
        newProcess->status = RUNNING;
    }
    //newProcess->next = *process_list;
    //*process_list = newProcess;
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


void freeProcessList(process* process_list){
    if(process_list != NULL){
        freeProcessList(process_list->next);
        freeCmdLines(process_list->cmd);
        free(process_list);
    }
}

void updateProcessStatus(process* process_list, int pid, int status){
    while(process_list != NULL){
        if(process_list->pid == pid){
            process_list->status = status;
            break;
        }
        process_list = process_list->next;
    }
}

void updateProcessList(process **process_list){
    int status;
    int pid;
    while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0){
        if(WIFEXITED(status) || WIFSIGNALED(status)){
            updateProcessStatus(*process_list, pid, TERMINATED);
        } 
        if(WIFSTOPPED(status)){
            updateProcessStatus(*process_list, pid, SUSPENDED);
        } 
        if(WIFCONTINUED(status)){
            updateProcessStatus(*process_list, pid, RUNNING);
        } 
    }
}

void removeAllTermianted(process** process_list){
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

void printProcessList(process** process_list){
    updateProcessList(process_list);
    process* curr = *process_list;
    printf("PID\tCommand\tSTATUS\n");
    while(curr != NULL){
        char* status = "RUNNING";
        if(curr->status == TERMINATED) status = "TERMINATED";
        if(curr->status == SUSPENDED) status = "SUSPENDED";
        printf("%d\t%s\t%s\n", curr->pid, curr->cmd->arguments[0], status);
        curr = curr->next;
    }       
    removeAllTermianted(process_list);
}

//receives a parsed line and invokes the program specified in the cmdLine using the proper system call.
void execute(cmdLine *pCmdLine){
    if(pCmdLine -> inputRedirect){
        fclose(stdin);
        if(fopen(pCmdLine ->inputRedirect,"r") == NULL){
            perror("Execution FAILED");
            _exit(1);
        }
    }

    if(pCmdLine -> outputRedirect){
        fclose(stdout);
        if(fopen(pCmdLine ->outputRedirect,"w") == NULL){
            perror("Execution FAILED");
            _exit(1);
        }
    }
    char *command = (*pCmdLine).arguments[0];
    if(execvp(command,(*pCmdLine).arguments) == -1){ // executing command + display error if execv failed.
        perror("Execution FAILED");
        _exit(1);
    }
}

void cd(cmdLine *pCmdLine){
    chdir(pCmdLine -> arguments[1]);
}

void makePipe(cmdLine *lineToParse){
    int pipefd[2]; // 0 read 1 write
    pid_t pid1, pid2;
    if(lineToParse->outputRedirect || lineToParse->next->inputRedirect){ // redirection
        fprintf(stderr, "Invalid redirection\n");
        return;
    }
    if (pipe(pipefd) == -1) {
        perror("Failed to pipe");
        _exit(-1);
    }

    pid1 = fork();
    if (pid1 == 0) { // child 1 process
        close(STDOUT_FILENO); // close standard output
        dup(pipefd[1]); // duplicate write-end of pipe
        close(pipefd[1]); // close write-end of pipe

        if(lineToParse -> inputRedirect){ // redirection
        fclose(stdin);
            if(fopen(lineToParse ->inputRedirect,"r") == NULL){
                perror("Execution FAILED");
                _exit(1);
            }
        }
        if(execvp(lineToParse->arguments[0],lineToParse->arguments) == -1){ // executing command + display error if execv failed.
            perror("Execution FAILED");
            _exit(1);
        }
    }else { // parent process
        close(pipefd[1]);
        if(lineToParse->next !=NULL){
            pid2 = fork();
            if (pid2 == 0) { // child 2 process
                close(STDIN_FILENO);
                dup(pipefd[0]);
                close(pipefd[0]);

                if(lineToParse->next -> outputRedirect){ // redirection
                    fclose(stdout);
                    if(fopen(lineToParse->next ->outputRedirect,"w") == NULL){
                        perror("Execution FAILED");
                        _exit(1);
                    }
                }
                if(execvp(lineToParse->next->arguments[0],lineToParse->next->arguments) == -1){ // executing command + display error if execv failed.
                    perror("Execution FAILED");
                    _exit(1);
                }

            } else {
                close(pipefd[0]);
                addProcess(&process_list, lineToParse, pid1);
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
            }
        }

    }
}

int main(int argc, char *argv[]){
    char cwd[PATH_MAX];
    char input[2048];
    struct cmdLine *lineToParse;
    char history[HISTLEN][2048];
    int newest = 0;
    int oldest = 0;
    int historySize = 0;
    for(int i=0; i<argc; i++){
		if(strcmp(argv[i],"-d") == 0)
			debug = 1;
	}

    while(1){
        int shouldFree = 1;
        if(getcwd(cwd, PATH_MAX) == 0) //print cwd
            return 1;
        printf("%s$ ", cwd);

        fgets(input,2048,stdin);
        if(strncmp("quit",input,4) == 0){ //end inf loop with "quit"
            freeProcessList(process_list);
            exit(0);
        }

        lineToParse = parseCmdLines(input); //parse input
        if(strcmp(lineToParse -> arguments[0],"history") ==0){
            for(int i = 0; i < historySize; i++){
                printf("%d) %s", i + 1, history[(oldest + i) % HISTLEN]);
            }
            freeCmdLines(lineToParse);
            continue;
        }
        else if(strcmp(lineToParse -> arguments[0],"!!") ==0){
            freeCmdLines(lineToParse);
            if(historySize < 1){
                fprintf(stderr, "History is empty!\n");
                continue;
            } else {
                lineToParse = parseCmdLines(history[(newest-1 + HISTLEN) % HISTLEN]);
            }
        }
        else if(lineToParse -> arguments[0][0] == '!'){
            int index = atoi(lineToParse -> arguments[0]+1) - 1;
            freeCmdLines(lineToParse);
            if(index >= historySize || index < 0){
                fprintf(stderr, "index %d is out of bound %d!\n", index, historySize);
                continue;
            } else {
                lineToParse = parseCmdLines(history[(oldest + index + HISTLEN) % HISTLEN]);
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
        if(lineToParse->next !=NULL){
            makePipe(lineToParse);
            shouldFree = 0;
        }
        else if(strcmp(lineToParse -> arguments[0],"cd") ==0){
            cd(lineToParse);        }
        else if(strcmp(lineToParse -> arguments[0],"wake") ==0){
            kill(atoi(lineToParse -> arguments[1]),SIGCONT);
        }
        else if(strcmp(lineToParse -> arguments[0],"suspend") ==0){
            kill(atoi(lineToParse -> arguments[1]),SIGTSTP);
        }
        else if(strcmp(lineToParse -> arguments[0],"kill") ==0) {
            kill(atoi(lineToParse -> arguments[1]),SIGINT);
        } else if(strcmp(lineToParse -> arguments[0],"procs") ==0) {
            printProcessList(&process_list);
        }
        else {
            //we create child process before execute
            shouldFree = 0;
            int pid = fork();
            if(pid != 0){// this is a parent
                if((*lineToParse).blocking != 0){ // if no &, we wait for child
                    waitpid(pid, NULL, 0);
                }
                addProcess(&process_list, lineToParse, pid);
            }else{ // this is a child
                execute(lineToParse); // DELETED CALL TO EXECUTE
            }
            if(debug == 1){
                fprintf(stderr,"PID: %d\n",pid);
                fprintf(stderr,"Executing command: %s\n",(*lineToParse).arguments[0]);
            }
        }
        if(shouldFree)
            freeCmdLines(lineToParse);
    }
    return 0;
}