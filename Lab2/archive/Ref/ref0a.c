#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linux/limits.h"
#include "LineParser.h"
#include <unistd.h>

//4.receives a parsed line and invokes the program specified in the cmdLine using the proper system call.
void execute(cmdLine *pCmdLine){
    char *command = (*pCmdLine).arguments[0];
    char path[PATH_MAX];
    strcat(path,command); //creating full path.
    if(execvp(path,(*pCmdLine).arguments) == -1){ // executing command.
        //5. display error if execv failed.
        perror("Execution FAILED");
        exit(1);
    }
}

int main(int argc, char *argv[]){
    char cwd[PATH_MAX];
    char input[2048];
    struct cmdLine *lineToParse;
    while(1){
        //1.print cwd
        if(getcwd(cwd, PATH_MAX) == 0)
            return 1;
        printf("cwd path: %s\n", cwd);

        //2.read line from user
        fgets(input,2048,stdin);

        //7.end inf loop with "quit"
        if(strncmp("quit",input,4) == 0)
            exit(0);

        //3.parse input
        lineToParse = parseCmdLines(input);

        execute(lineToParse);
        freeCmdLines(lineToParse);
    }
    return 0;
}