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
    int VIRUS_NAME_SIZE = 16;
    virus* v = (virus*) malloc(sizeof(virus));

    // Read the signature size from the binary file
    fread(&v->sigSize, sizeof(unsigned short), 1, inFile);
    
    // Read the virus name from the binary file
    fread(v->virusName, sizeof(char), VIRUS_NAME_SIZE, inFile);
    v->virusName[VIRUS_NAME_SIZE] = '\0';
    
    int SIG_SIZE =v->sigSize;
        if(SIG_SIZE==0) return NULL;
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


/* linked list stuff:*/

typedef struct link link;
typedef struct link {
link *nextVirus;
virus *vir;
}link;


void list_print(link *virus_list, FILE* outFile) {
    while (virus_list != NULL) {
        printVirus(virus_list->vir,outFile);
        virus_list = virus_list->nextVirus; /* Move to the next link. */
    }
}

link* list_append(link* virus_list, virus* data) {
    link* new_link = (link*) malloc(sizeof(link)); /* Allocate memory for the new link. */
    new_link->nextVirus = NULL; /* Set the next pointer to NULL, since this will be the last link. */
    new_link->vir = data; /* Set the data of the new link. */
    
    if (virus_list == NULL) { /* If the list is empty, return the new link. */
        return new_link;
    }
    
    link* current_link = virus_list;
    while (current_link->nextVirus != NULL) { /* Find the last link in the list. */
        current_link = current_link->nextVirus;
    }
    
    current_link->nextVirus = new_link; /* Set the next pointer of the last link to the new link. */
    return virus_list; /* Return the first link in the list. */
}

void list_free(link *virus_list) {
    link *current_link = virus_list;
    while (current_link != NULL) {
        link *temp_link = current_link; /* Store a reference to the current link. */
        current_link = current_link->nextVirus; /* Move to the next link before freeing the current link. */
        free(temp_link->vir); /* Free the memory allocated for the virus struct. */
        free(temp_link); /* Free the memory allocated for the current link. */
    }
}





int parseFile(FILE* inFile,link* virus_list){
    int MAGIC_NUM_SIZE = 4;
    char magicNum[MAGIC_NUM_SIZE+1];
    // Read the magic number from the binary file
    fread(magicNum, sizeof(char), MAGIC_NUM_SIZE, inFile);
    magicNum[MAGIC_NUM_SIZE] = '\0';
    printf("\t~DEBUG: magicNum is: %s\n",magicNum); //###DEBUG
    printf("\t~DEBUG: sizeof(magicNum) is: %i\n",sizeof(magicNum)); //###DEBUG
        /* doesnot compares correctly,need to fix*/
    if(strcmp("VISL", magicNum) != 0){
        printf("magicNum is: %s, exiting program\n",magicNum);
        return -1;  //wrong file prefix
    }
    
    virus* v=NULL;
    do{
        v = readVirus(inFile);
        list_append(virus_list,v);
        printVirus(v,stdout);
        printf("\n\n");
    }while(v!=NULL);
    return 1;//finished sucssessfully
}




int main(int argc, char** argv) {
    FILE* outFile = stdout;
    link* virus_list=NULL;
    if (argc != 2) {
        printf("error, exiting program\n");
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    parseFile(fp,virus_list);
    // virus* v = readVirus(fp);
    // outFile=fopen("test","w");
    //             if (outFile == NULL)
    //                 fprintf(stderr, "Error: could not open output file\nExiting program!\n\n");     
    // if(v!=NULL) printVirus(v,stdout);      //change stdout to outfile

    return 0;
}
