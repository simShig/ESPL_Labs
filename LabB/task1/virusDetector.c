#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

void debugPrintRead(unsigned char* signature){
        unsigned short sigSize;
        printf("UNcHANGED bytes are- [4]: %02X, [5]: %02X\n",signature[4],signature[5]);
        sigSize = signature[4]<<8|signature[5];     //convert small indian hexa to dec, refferance: https://stackoverflow.com/questions/19275955/convert-little-endian-to-big-endian
        printf("sigSize is: %i\n\n",sigSize);
        for (int i = 0; i < 1000; i++)
        {
            printf("%02X ",signature[i]);  //~~DEBUG!!!
        }
        printf("\n");  //~~DEBUG!!!
        for (int i = 0; i < 1000; i++)
        {
            printf("%c ",signature[i]);  //~~DEBUG!!!
        }
        printf("\n");  //~~DEBUG!!!
}



// Reads the next virus from the given file pointer
virus* readVirus(FILE* inFile) {
    char inChar;
    long filesize = 0;
    /*~~1. transport file content to buffer~~*/

    // Get the file size
    while((inChar = fgetc(inFile)) != EOF){
        filesize++;
    }
    fseek(inFile, 0, SEEK_SET); //had troubles returnning to beginning of file (after the while fgetc), found better way then closing and opening again on stackOverFlow:
                                // https://stackoverflow.com/questions/32366665/resetting-pointer-to-the-start-of-file

    // printf("filesize:%li\n",filesize);

    // Allocate a buffer for the file content
    char *buffer = (char *) malloc(filesize);
    printf("\t~~DEBUG~~ bufferSize is: %i\n",sizeof(buffer));  //##########DEBUG!!!#############
    if (!buffer) {
        fprintf(stderr, "Error: could not allocate memory\n");
        fclose(inFile);
        return NULL;
    }
    printf("file size is: %li \n",filesize);

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, filesize, inFile);
    if (bytes_read != filesize) {
        fprintf(stderr, "bytesRead: %i ,Error: could not read entire file\n",bytes_read);
        fclose(inFile);
        free(buffer);
        return NULL;
    }


    /*~~2. read from buffer according to virus-stractures and allocate to virus's vars~~*/
    char magicNum[4];
    unsigned short SigSize[2];
    int size = buffer[4]<<8|buffer[5];     //convert small indian hexa to dec, refferance: https://stackoverflow.com/questions/19275955/convert-little-endian-to-big-endian
    char virusName[16];
    unsigned char* sig=NULL;
    virus* v = malloc(sizeof(virus));

    for (int i = 0; i < 4; i++) {
        magicNum[i]=(unsigned char)buffer[i];
    }
    printf("magicNum is:%s\n",magicNum);
    for (int i = 0; i < 2; i++) {
        SigSize[i]=buffer[i+4];
    }
    for (int i = 0; i < 16; i++) {
        virusName[i]=buffer[i+6];
    }

    // sig =(unsigned char*)buffer[22];
    for (int i = 0; i < size; i++) {
        printf("\t~~DEBUG~~ i is: %i\n",i);  //##########DEBUG!!!#############
        sig[i]=buffer[i+22];
    }
    memcpy(&v->SigSize, SigSize, sizeof(v->SigSize));
    memcpy(v->virusName, virusName, sizeof(v->virusName));
    memcpy(v->sig,sig , size);
    return v;
  
    // Clean up
    free(buffer);
    fclose(inFile);
}


// Prints the given virus to the given file pointer
void printVirus(virus* v, FILE* output) {
    fprintf(output, "Virus name: %s\n", v->virusName);
    fprintf(output, "Virus size: %d\n", v->SigSize);
    fprintf(output, "Signature:\n");

    // Print the virus signature in hexadecimal format
    for (int i = 0; i < v->SigSize; i++) {
        fprintf(output, "%02X ", v->sig[i]);
    }
    fprintf(output, "\n");
}


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "rb");
    if (!fp) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    virus* v = readVirus(fp);
    printVirus(v,stdout);


    return 0;
}
