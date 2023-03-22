#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char updateChar(char curr, int lower, int upper,int addenc){ // func that gets the upper and lower bounds and returns the updated char
    if(lower<=curr && curr<=upper){
        char newChar = curr + addenc;
        if(newChar > upper){
            newChar = newChar % upper + lower - 1;
        }else if(newChar < lower){
            newChar = upper - lower + 1 + newChar;
        }
            return newChar;
    }
    return curr;
}
int main(int argc, char **argv) {
    int debugMode =0; //0 = false, 1 = true
    int i;
    FILE* outFile = stdout;
    FILE* inFile = stdin;
    char curr;
    char encType = 1; // 1 = plus, -1 = minus
    char *numbers; // contains the substring after +/-e

    for(i=1; i<argc; i++){ 
        if(strcmp(argv[i],"+D")==0){
            debugMode = 1;
        }
        else if(strcmp(argv[i],"-D")==0){
            debugMode = 0;
        }
        else if(strncmp(argv[i],"+e", 2)==0){
            encType = 1;
            numbers = argv[i]+2;
        }
        else if(strncmp(argv[i],"-e", 2)==0){
            encType = -1;
            numbers = argv[i]+2;
        }
        if(debugMode == 1 && i < argc-1 && !(strcmp(argv[i+1],"-D")==0)) // printing info without +-D
            fprintf(stderr,"%s\n",argv[i+1]);
    }
    int counter = 0;
    while((curr = fgetc(inFile)) != EOF){
        if(encType !=0 && curr !='\0'){
            curr = updateChar(curr, 'A', 'Z', (numbers[counter] - '0')*encType); // uppercase (why -'0'? numbers[counter] contains ascii of digit because of char type)
            curr = updateChar(curr, 'a', 'z', (numbers[counter] - '0')*encType); // lowecase
            curr = updateChar(curr, '0', '9', (numbers[counter] - '0')*encType); // digits
            fputc(curr, outFile);
            // printf("%c",curr);
            counter++;
            if((numbers[counter]) == '\0'){ // if we got to the last digit in numbers we loop back to the start
                counter = 0;
            }
        }
        char c;
        if ((c = fgetc(outFile)) != EOF) { //print from outFile
            putchar(c);
        }
    }

    return 0;
}
