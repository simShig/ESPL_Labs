#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linux/limits.h"
#include "LineParser.h"
#include <unistd.h>

int debug = 0;
//4.receives a parsed line and invokes the program specified in the cmdLine using the proper system call.
void execute(cmdLine *pCmdLine){
    char *command = (*pCmdLine).arguments[0];
    if(execvp(command,(*pCmdLine).arguments) == -1){ // executing command + display error if execv failed.
        perror("Execution FAILED");
        _exit(1);
    }
}

int main(int argc, char *argv[]){
    char cwd[PATH_MAX];
    char input[2048];
    struct cmdLine *lineToParse;

    for(int i=0; i<argc; i++){
		if(strcmp(argv[i],"-d") == 0)
			debug = 1;
	}

    while(1){

        if(getcwd(cwd, PATH_MAX) == 0) //print cwd
            return 1;
        printf("cwd path: %s\n", cwd);

        fgets(input,2048,stdin);
        if(strncmp("quit",input,4) == 0) //end inf loop with "quit"
            exit(0);

        lineToParse = parseCmdLines(input); //parse input
        if(debug == 1){
            fprintf(stderr,"PID: %d\n",getpid());
            fprintf(stderr,"Executing command: %s\n",(*lineToParse).arguments[0]);
        }
        //we create child process before execute
        if(fork() ==0){
            execute(lineToParse);
        }
        freeCmdLines(lineToParse);
    }
    return 0;
}