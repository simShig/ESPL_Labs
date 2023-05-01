#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process {
    cmdLine* cmd;
    pid_t pid;
    int status;
    struct process* next;
} process;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
    process* new_process = (process*)malloc(sizeof(process));
    new_process->cmd = cmd;
    new_process->pid = pid;
    new_process->status = RUNNING;
    new_process->next = *process_list;
    *process_list = new_process;
}

void printProcessList(process** process_list) {
    printf("PID\tCommand\t\t\tSTATUS\n");
    printf("---\t-------\t\t\t------\n");
    process* curr = *process_list;
    int i = 1;
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
        i++;
    }
}

void freeProcessList(process** process_list) {
    process* curr = *process_list;
    while (curr != NULL) {
        process* temp = curr;
        curr = curr->next;
        freeCmdLines(temp->cmd);
        free(temp);
    }
    *process_list = NULL;
}
