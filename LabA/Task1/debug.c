#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv) {

    int isDebug =0; //0 == false
    int i;
    for(i=1; i<argc; i++){
        // printf("arg num %d, and value is: %s\n",i,argv[i]);
        printf("\tisDebug: %d\n",isDebug);
        char* arg = argv[i];        
        char firstChar = arg[0];
        // printf("firstChar is %d\n",firstChar);
        if (firstChar=='+'){
            if(arg[1]=='D'){
                isDebug=1;
                continue;
            }
        }else if (firstChar=='-'){
            if(arg[1]=='D'){
                isDebug=0;
                continue;
            }
        }
        if (isDebug==1) fprintf(stderr,"%s\n",argv[i]);
        // printf("made it to end of loop %d,debug is %i\n",i,isDebug);
    }
}
