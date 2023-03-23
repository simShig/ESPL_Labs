#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*method to get key length - relevant for cyclyc encription*/
int getKeyLength (char* arg){
    int len = 0;
    int j=2;
    while (arg[j]!='\0')
    {
     
        j++;
        len++;
    }
    return len;
}

/*one 'general' encryption method instead of 3 different (case a~z\A~Z\1~9)*/
char encrypt(char inChar, int encKeyTypeProduct, int from, int to){
    char outChar=inChar;
    if(from<=inChar&&to>=inChar){
        outChar = inChar+encKeyTypeProduct;
        if(outChar<from)outChar=outChar+to-from+1;
        else if(outChar>to) outChar=outChar-to+from-1;        
    }//if in bound
    return outChar;
}

int main(int argc, char **argv) {
    int i;
    //debugger vars:
    int isDebug =0; //0 == false
    //encoder vars:
    FILE* outFile = stdout;
    FILE* inFile = stdin;
    int isEncoding = 0;
    int encType = 0 ;    //-/+, represented by -1/+1
    char *encKey= NULL;
    int keyCounter=0;   
    int keyLen=0;
    char inChar;
    char outChar;

   //loop to identify flags:
    for(i=1; i<argc; i++){
        // printf("arg num %d, and value is: %s\n",i,argv[i]);
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
            }
            else if(arg[1]=='i'){
                inFile=fopen(argv[i]+2,"r");
                if (inFile == NULL)
                    fprintf(stderr, "Error: could not open input file %s\n", argv[i]+2);
            }
            else if(arg[1]=='o'){
                outFile=fopen(argv[i]+2,"w");
                if (outFile == NULL)
                    fprintf(stderr, "Error: could not open output file %s\n", argv[i]+2);
            }

        }
        if (isDebug==1) fprintf(stderr,"%s\n",argv[i]);
        
    }//finished line-args loop
    
    
    //starting encription loop:
    while((inChar = fgetc(inFile)) != EOF){
        if(inChar !='\0' && isEncoding==1){
            if(inChar>='A'&&inChar<='Z')
                inChar = encrypt(inChar,(encKey[keyCounter]-48)*encType,'A','Z');
            if(inChar>='a'&&inChar<='z')
                inChar = encrypt(inChar,(encKey[keyCounter]-48)*encType,'a','z');
            if(inChar>='0'&&inChar<='9')
                inChar = encrypt(inChar,(encKey[keyCounter]-48)*encType,'0','9');
            fputc(inChar, outFile);
            keyCounter++;
            if(keyCounter>=keyLen){ //for cyclyc encription over the key
                keyCounter = 0;
            }
        }
        if ((outChar = fgetc(outFile)) != EOF) { //use outFile
            putchar(outChar);
        }
    }

    /* if opened - close files */
    if (inFile != stdin) {
        fclose(inFile);
    }
    if (outFile != stdout) {
        fclose(outFile);
    }
    return 0;
}
