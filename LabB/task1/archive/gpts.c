// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #define MAGIC_NUM_SIZE 4
// #define VIRUS_NAME_SIZE 16
// #define SIG_SIZE 40

// int main(int argc, char** argv) {
//     char magicNum[MAGIC_NUM_SIZE+1];
//     unsigned short sigSize;
//     char virusName[VIRUS_NAME_SIZE+1];
//     unsigned char* sig = (unsigned char*) malloc(SIG_SIZE * sizeof(unsigned char));
    
//     FILE* fp = fopen("signatures-B", "r");
//     if (fp == NULL) {
//         printf("Error opening file\n");
//         return 1;
//     }
    
//     // Read the magic number from the binary file
//     fread(magicNum, sizeof(char), MAGIC_NUM_SIZE, fp);
//     magicNum[MAGIC_NUM_SIZE] = '\0';
    
//     // Read the signature size from the binary file
//     fread(&sigSize, sizeof(unsigned short), 1, fp);
    
//     // Read the virus name from the binary file
//     fread(virusName, sizeof(char), VIRUS_NAME_SIZE, fp);
//     virusName[VIRUS_NAME_SIZE] = '\0';
    
//     // Read the signature from the binary file
//     fread(sig, sizeof(unsigned char), SIG_SIZE, fp);
    
//     // Close the file
//     fclose(fp);
    
//     // Print the values
//     printf("Magic number: %s\n", magicNum);
//     printf("Signature size: %hu\n", sigSize);
//     printf("Virus name: %s\n", virusName);
//     printf("Signature: ");
//     for (int i = 0; i < SIG_SIZE; i++) {
//         printf("%02x ", sig[i]);
//     }
//     printf("\n");
    
//     // Free memory allocated for signature
//     free(sig);
    
//     return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC_NUM_SIZE 4
#define VIRUS_NAME_SIZE 16
#define SIG_SIZE 40

typedef struct {
    char magicNum[MAGIC_NUM_SIZE+1];
    unsigned short sigSize;
    char virusName[VIRUS_NAME_SIZE+1];
    unsigned char* sig;
} Virus;

Virus* readVirus(FILE* inFile) {
    Virus* virus = (Virus*) malloc(sizeof(Virus));
    virus->sig = (unsigned char*) malloc(SIG_SIZE * sizeof(unsigned char));
    
    // Read the magic number from the binary file
    fread(virus->magicNum, sizeof(char), MAGIC_NUM_SIZE, inFile);
    virus->magicNum[MAGIC_NUM_SIZE] = '\0';
    
    // Read the signature size from the binary file
    fread(&virus->sigSize, sizeof(unsigned short), 1, inFile);
    
    // Read the virus name from the binary file
    fread(virus->virusName, sizeof(char), VIRUS_NAME_SIZE, inFile);
    virus->virusName[VIRUS_NAME_SIZE] = '\0';
    
    // Read the signature from the binary file
    fread(virus->sig, sizeof(unsigned char), SIG_SIZE, inFile);
    
    return virus;
}

int main(int argc, char** argv) {
    const char *filename = argv[1];

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    
    // Read a virus from the binary file
    Virus* virus = readVirus(fp);
    
    // Close the file
    fclose(fp);
    
    // Print the values
    printf("Magic number: %s\n", virus->magicNum);
    printf("Signature size: %hu\n", virus->sigSize);
    printf("Virus name: %s\n", virus->virusName);
    printf("Signature: %s\n",virus->sig);
    for (int i = 0; i < SIG_SIZE; i++) {
        printf("%02x ", virus->sig[i]);
    }
    printf("\n");
    
    // Free memory allocated for signature
    // free(sig);
    
    return 0;

}