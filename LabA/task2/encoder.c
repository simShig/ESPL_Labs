#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv) {
    //debugger vars:
    int isDebug =0; //0 == false
    //encoder vars:
    int isEncoding = 0;
    int encType = 0 ;    //-/+, represent by -1/+1
    char *encKey= NULL;
    int keyCounter=0;   //counting
    int i;
    //loop to identify flags:
    for(i=1; i<argc; i++){
        // printf("arg num %d, and value is: %s\n",i,argv[i]);
        printf("\tisDebug: %d\n",isDebug);
        char* arg = argv[i];        
        char firstChar = arg[0];
        // int j=2;
        // while (arg[j]!='\0')
        // {
        //     printf("%c\n",arg[j]);
        //     j++;
        // }
        // encKey=argv[i]+2;   //points to the 2 'cell' (adress) from the start, gets the suffix of the arg
        // printf("%s\n",encKey);
        
        // printf("firstChar is %d\n",firstChar);
        if (firstChar=='+'){
            if(arg[1]=='D'){
                isDebug=1;
                continue;
            }
            else if(arg[1]=='e'){
                isEncoding=1;
                encType=1;
                encKey=argv[i]+2;   //points to the 2 'cell' (adress) from the start, gets the suffix of the arg
                continue;
            }
        }else if (firstChar=='-'){
            if(arg[1]=='D'){
                isDebug=0;
                continue;
            }
            else if(arg[1]=='e'){
                isEncoding=1;
                encType=-1;
                encKey=argv[i]+2;   //points to the 2 'cell' (adress) from the start, gets the suffix of the arg
                continue;
            }
        }
        if (isDebug==1) fprintf(stderr,"%s\n",argv[i]);
        
    }//finished line-args loop

//starting encription loop:




}
