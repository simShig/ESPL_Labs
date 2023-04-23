#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int fd[2];      //fd - file descriptor
    pid_t pid;
    char buf[100];          //for parent
    char message[100];      //for child - user input

    if (pipe(fd) == -1) {       //case PIPE ERROR
        printf("pipe comunication error\n");
        exit(EXIT_FAILURE);
    }


    pid = fork();

    if (pid == -1) {        //case FORK ERROR
        printf("fork creation error\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { //child proccess
        close(fd[0]); // Close unused read end (stdin)
        printf("Enter message to send: ");
        fgets(message, 100, stdin); // Get user input for message
        write(fd[1], message, 100);
        close(fd[1]);
        exit(EXIT_SUCCESS);
    } else {
        close(fd[1]); // Close unused write end (stdout)
        read(fd[0], buf, 100);
        printf("Message received: %s\n", buf);
        close(fd[0]);
        wait(NULL); // Wait for child process to terminate
        exit(EXIT_SUCCESS);
    }
}
