#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
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
        // execute "ls -l"
        char* args[] = {"ls", "-l", NULL};
        fprintf(stderr, "(child1>going to execute cmd: 'ls -l')\n");
        if(execvp(args[0], args)==-1){
            perror("execvp ERROR");
            exit(EXIT_FAILURE);
        }
    } else {
        // Step 4: in parent process
        // close the write end of the pipe
        close(pipefd[1]);
        fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
    }

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
        // execute "tail -n 2"
        char* args[] = {"tail", "-n", "2", NULL};
        fprintf(stderr, "(child2>going to execute cmd: 'tail -n 2')\n");
        if(execvp(args[0], args)==-1){
            perror("execvp ERROR");   
            exit(EXIT_FAILURE);
        }
    } else {
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

    return 0;
}



/*
LAB-C QUESTIONS:
1. commenting out step 4:  will not redirect the stdout = will not print commands by child1
            also will not exit the proccess
            
            ~~ If you comment out step 4 in the code (i.e., do not close the write end of the pipe in the parent process), 
            then the second child process will not receive the end-of-file indicator from the pipe when the 
            first child process finishes writing to the pipe. This may cause the second child process to hang indefinitely,
            waiting for input from the pipe that will never come.~~

    will skip printing:
            (child1>redirecting stdout to the write end of the pipe...)
            ...
            -rw-r--r-- 1 root root  2947 Apr 24 17:44 mypipeline.c
            -rw-r--r-- 1 root root  7516 Apr 24 17:44 mypipeline.o

2. commenting out step 7: then the parent process will keep the read end of the pipe open. 
        This may cause the program to hang indefinitely, waiting for the second child process to finish reading from the pipe,
        even after the first child process has already finished executing.
        since the write end of the pipe has already been closed by the parent process in step 4.
        This would cause the program to become unresponsive and eventually lead to a deadlock.


    will skip printing:
        (parent_process>closing the read end of the pipe...)

3. commenting out steps 4 and 8: then the parent process will keep both ends of the pipe open.
        This may cause the program to hang indefinitely, waiting for the second child process to finish reading from the pipe,
        even after the first child process has already finished executing.
        Additionally, the write end of the pipe will not be closed, which may cause the first child process to continue running
        and writing to the pipe indefinitely.
        In addition, if the child processes are still running when the parent process exits, they become orphan processes,
        which may continue running until they are terminated by the system or another process.

*/