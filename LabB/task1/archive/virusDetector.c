#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

// Reads the next virus from the given file pointer
virus* readVirus(FILE* file) {
    // Read the virus signature length
    unsigned short sigSize;
    if (fread(&sigSize, sizeof(sigSize), 1, file) != 1) {
        return NULL; // Error or end of file
    }
    sigSize = letoh16(sigSize); // Convert from little endian to host byte order

    // Read the virus name
    char virusName[17];
    if (fread(virusName, sizeof(char), 16, file) != 16) {
        return NULL; // Error or end of file
    }
    virusName[16] = '\0';

    // Allocate memory for the virus structure and signature
    virus* v = malloc(sizeof(virus));
    v->SigSize = sigSize;
    memcpy(v->virusName, virusName, sizeof(v->virusName));
    v->sig = malloc(sigSize);

    // Read the virus signature
    if (fread(v->sig, sizeof(unsigned char), sigSize, file) != sigSize) {
        free(v->sig);
        free(v);
        return NULL; // Error or end of file
    }

    return v;
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
