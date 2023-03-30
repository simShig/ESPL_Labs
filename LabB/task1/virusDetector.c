#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short sigSize;
    char virusName[16];
    unsigned char* sig;
} virus;


//     int size = buffer[4]<<8|buffer[5];     //convert small indian hexa to dec, refferance: https://stackoverflow.com/questions/19275955/convert-little-endian-to-big-endian

// Reads the next virus from the given file pointer
virus* readVirus(FILE* inFile) {
    int MAGIC_NUM_SIZE = 4;
    int VIRUS_NAME_SIZE = 16;
    virus* v = (virus*) malloc(sizeof(virus));
    char magicNum[MAGIC_NUM_SIZE+1];
    // Read the magic number from the binary file
    fread(magicNum, sizeof(char), MAGIC_NUM_SIZE, inFile);
    magicNum[MAGIC_NUM_SIZE] = '\0';
    if(strcmp("VISL",magicNum)!=0){
        printf("magicNum is: %s, exiting program\n",magicNum);
        return NULL;
    }

    // Read the signature size from the binary file
    fread(&v->sigSize, sizeof(unsigned short), 1, inFile);
    
    // Read the virus name from the binary file
    fread(v->virusName, sizeof(char), VIRUS_NAME_SIZE, inFile);
    v->virusName[VIRUS_NAME_SIZE] = '\0';
    
    int SIG_SIZE =v->sigSize;
    v->sig = (unsigned char*) malloc(SIG_SIZE * sizeof(unsigned char));
    // Read the signature from the binary file
    fread(v->sig, sizeof(unsigned char), SIG_SIZE, inFile);
    
    return v;
}




// Prints the given virus to the given file pointer
void printVirus(virus* v, FILE* output) {
    fprintf(output, "Virus name: %s\n", v->virusName);
    fprintf(output, "Virus size: %d\n", v->sigSize);
    fprintf(output, "Signature:\n");

    // Print the virus signature in hexadecimal format
    for (int i = 0; i < v->sigSize; i++) {
        fprintf(output, "%02X ", v->sig[i]);
    }
    fprintf(output, "\n");
}


int main(int argc, char** argv) {
    FILE* outFile = stdout;
    if (argc != 2) {
        printf("error, exiting program\n");
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    virus* v = readVirus(fp);
    outFile=fopen("test","w");
                if (outFile == NULL)
                    fprintf(stderr, "Error: could not open output file\nExiting program!\n\n");     
    if(v!=NULL) printVirus(v,outFile);

    return 0;
}
