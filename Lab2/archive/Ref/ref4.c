#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){
    int fd[2]; // making the pipe. 0=read 1=write
    char *msg = "hello";
    int msglen = strlen(msg);
    pid_t cpid;
    char buf[BUFSIZ];
    if(pipe(fd) == -1){
        fprintf(stderr,"Pipe failed");
        return 1;
    }
    //copied code from pipe man with little changes
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(1);
    }
    if (cpid == 0) {    /* Child reads from pipe */
        close(fd[0]);          /* Close unused write end */
        write(fd[1], msg, msglen);
        close(fd[1]);
        _exit(0);

    } else {            /* Parent writes to pipe */
        close(fd[1]);          /* Close unused read end */
        read(fd[0],&buf,BUFSIZ);
        printf("%s\n",buf);
        close(fd[0]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */
        exit(0);
    }

    return 0;
}