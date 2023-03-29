#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC_NUM_SIZE 4
#define VIRUS_NAME_SIZE 16
#define SIG_SIZE 40

int main(int argc, char** argv) {
    char magicNum[MAGIC_NUM_SIZE+1];
    unsigned short sigSize;
    char virusName[VIRUS_NAME_SIZE+1];
    unsigned char* sig = (unsigned char*) malloc(SIG_SIZE * sizeof(unsigned char));
    
    FILE* fp = fopen("signatures-B", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    
    // Read the magic number from the binary file
    fread(magicNum, sizeof(char), MAGIC_NUM_SIZE, fp);
    magicNum[MAGIC_NUM_SIZE] = '\0';
    
    // Read the signature size from the binary file
    fread(&sigSize, sizeof(unsigned short), 1, fp);
    
    // Read the virus name from the binary file
    fread(virusName, sizeof(char), VIRUS_NAME_SIZE, fp);
    virusName[VIRUS_NAME_SIZE] = '\0';
    
    // Read the signature from the binary file
    fread(sig, sizeof(unsigned char), SIG_SIZE, fp);
    
    // Close the file
    fclose(fp);
    
    // Print the values
    printf("Magic number: %s\n", magicNum);
    printf("Signature size: %hu\n", sigSize);
    printf("Virus name: %s\n", virusName);
    printf("Signature: ");
    for (int i = 0; i < SIG_SIZE; i++) {
        printf("%02x ", sig[i]);
    }
    printf("\n");
    
    // Free memory allocated for signature
    free(sig);
    
    return 0;
}
