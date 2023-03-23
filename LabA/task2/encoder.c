#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*method to get key length - relevant for cyclyc encription*/
int getKeyLength (char* arg){
    int len = 0;
    int j=2;
    while (arg[j]!='\0')
    {
        // printf("keyChar is: %c\n",arg[j]);
        j++;
        len++;
    }
    // printf("len is:%i\n",len);
    return len;
}

/*one 'general' encryption method instead of 3 different (case a~z\A~Z\1~9)*/
char encrypt(char inChar, int encKeyTypeProduct, int from, int to){
    // printf("inChar is:%c ,enckeyType is:%i,from %i to %i\n",inChar,encKeyTypeProduct,from,to);
    if(from<=inChar&&to>=inChar){
        char outChar = inChar+encKeyTypeProduct;
        // printf("\tnot balanced outChar is: %c (%i)\n",outChar,outChar);
        if(outChar<from)outChar=outChar+to-from+1;
        else if(outChar>to) outChar=outChar-to+from-1;        
        // printf("\toutChar is: %c (%i)\n",outChar,outChar);
    }//if in bound

}

/*detect if char legal for encoding,and what type: 1:0~9|2:a~z|3:A~Z|0:else*/
int typeOfChar(char inChar){
    if(inChar>='0'&&inChar<='9') return 1;
    if(inChar>='a'&&inChar<='z') return 2;
    if(inChar>='A'&&inChar<='Z') return 3;
    else return 0;

}

int main(int argc, char **argv) {
    int i;
    //debugger vars:
    int isDebug =0; //0 == false
    //encoder vars:
    FILE* outFile = stdout;
    FILE* inFile = stdin;
    int isEncoding = 0;
    int encType = 0 ;    //-/+, represent by -1/+1
    char *encKey= NULL;
    int keyCounter=0;   //counting
    int keyLen=0;
    char inChar=NULL;
    char outChar=NULL;

    //loop to identify flags:
    for(i=1; i<argc; i++){
        // printf("arg num %d, and value is: %s\n",i,argv[i]);
        // printf("\tisDebug: %d\n",isDebug);
        char* arg = argv[i];        
        char firstChar = arg[0];
        
        if (firstChar=='+'){
            if(arg[1]=='D'){
                isDebug=1;
                continue;
            }
            else if(arg[1]=='e'){
                isEncoding=1;
                encType=1;
                encKey=argv[i]+2;   //points to the 2 'cell' (adress) from the start, gets the suffix of the arg
                keyLen = getKeyLength(arg);
                // printf("encKey size:%i\n",keyLen);
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
                keyLen = getKeyLength(arg);
                // printf("encKey size:%i\n",keyLen);
                continue;
            }
        }
        if (isDebug==1) fprintf(stderr,"%s\n",argv[i]);
        
    }//finished line-args loop

//starting encription loop:
        while((inChar = fgetc(inFile)) != EOF){
            int flag = typeOfChar(inChar);
            if(flag){
                switch (flag)
                {
                case 1:
                    outChar=encrypt(inChar,(encKey[keyCounter]-48)*encType,'0','9');
                    break;
                case 2:
                    outChar=encrypt(inChar,(encKey[keyCounter]-48)*encType,'a','z');
                    break;
                case 3:
                    outChar=encrypt(inChar,(encKey[keyCounter]-48)*encType,'A','Z');
                    break;
                default:
                    break;
                }
                fputc(outChar, outFile);
                keyCounter++;
                if (keyCounter==keyLen) keyCounter=0;
            }
            if((outChar = fgetc(outFile)) != EOF) putchar(outChar);
            printf("\n");
        }
}
