#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
    struct virus* next; //not in the original instructions
} virus;

virus* readVirus(FILE* fp);
void printVirus(virus* virus, FILE* output);
void printHex(const char *buffer, size_t length);

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

    virus* head = NULL;
    virus* curr = NULL;

    while (1) {
        virus* v = readVirus(fp);
        if (!v) {
            break;
        }

        if (!head) {
            head = v;
        } else {
            curr->next = v;
        }

        curr = v;
    }

    fclose(fp);

    curr = head;
    while (curr) {
        printVirus(curr, stdout);
        curr = curr->next;
    }

    return 0;
}

virus* readVirus(FILE* fp) {
    unsigned short SigSize;
    char virusName[16];

    size_t n = fread(&SigSize, sizeof(SigSize), 1, fp);
    if (n != 1) {
        return NULL;
    }

    n = fread(virusName, sizeof(virusName), 1, fp);
    if (n != 1) {
        return NULL;
    }

    virus* v = (virus*)malloc(sizeof(virus));
    v->SigSize = SigSize;
    memcpy(v->virusName, virusName, sizeof(virusName));
    v->sig = (unsigned char*)malloc(SigSize);
    n = fread(v->sig, SigSize, 1, fp);
    if (n != 1) {
        free(v->sig);
        free(v);
        return NULL;
    }

    v->next = NULL;

    return v;
}

void printVirus(virus* v, FILE* output) {
    fprintf(output, "Virus name: %s\n", v->virusName);
    fprintf(output, "Virus size: %d\n", v->SigSize);
    fprintf(output, "Virus signature:\n");
    printHex((const char*)v->sig, v->SigSize);
    fprintf(output, "\n");
}

void printHex(const char *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        fprintf(stdout, "%02x ", (unsigned char)buffer[i]);
    }
}
