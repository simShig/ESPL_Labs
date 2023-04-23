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
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Step 2: fork the first child process (child1)
    child1_pid = fork();
    if (child1_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child1_pid == 0) {
        // Step 3: in child1 process
        // close the standard output
        close(STDOUT_FILENO);
        // duplicate the write-end of the pipe
        dup(pipefd[1]);
        // close the file descriptor that was duplicated
        close(pipefd[1]);
        // execute "ls -l"
        char* args[] = {"ls", "-l", NULL};
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
        fprintf(stderr, "(child1>going to execute cmd: 'ls -l')\n");
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Step 4: in parent process
        // close the write end of the pipe
        close(pipefd[1]);
        fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
    }

    // Step 5: fork the second child process (child2)
    child2_pid = fork();
    if (child2_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child2_pid == 0) {
        // Step 6: in child2 process
        // close the standard input
        close(STDIN_FILENO);
        // duplicate the read-end of the pipe
        dup(pipefd[0]);
        // close the file descriptor that was duplicated
        close(pipefd[0]);
        // execute "tail -n 2"
        char* args[] = {"tail", "-n", "2", NULL};
        fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
        fprintf(stderr, "(child2>going to execute cmd: 'tail -n 2')\n");
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
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
