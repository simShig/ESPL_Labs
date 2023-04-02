#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short sigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

//     int size = buffer[4]<<8|buffer[5];     //convert small indian hexa to dec, refferance: https://stackoverflow.com/questions/19275955/convert-little-endian-to-big-endian


int parseFile(FILE* inFile){
    int MAGIC_NUM_SIZE = 4;
    char magicNum[MAGIC_NUM_SIZE+1];
    // Read the magic number from the binary file
    fread(magicNum, sizeof(char), MAGIC_NUM_SIZE, inFile);
    magicNum[MAGIC_NUM_SIZE] = '\0';
    if(strcmp("VISL",magicNum)!=0){
        printf("magicNum is: %s, exiting program\n",magicNum);
        return -1;  //wrong file prefix
    }
    int finished=1;
    long check = ftell(inFile);
    printf("filecheck is: %li",check);
    while (fseek(inFile, 0L, SEEK_END)!=0)
    {
        virus* v=readVirus(inFile);
        printVirus(v,stdout);
    }
    
}

// Reads the next virus from the given file pointer
virus* readVirus(FILE* inFile) {
        printf("\tDEBUG!!!\n");         //@@@@@DEBUG@@@@
    int VIRUS_NAME_SIZE = 16;
    virus* v = (virus*) malloc(sizeof(virus));

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

    return NULL;
}

/* task 1b:*/

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


/* menu part: */

virus*  quit(){
    return NULL;
}

virus*  detect(){
    return NULL;
}
virus*  fix(){
    return NULL;
}




struct fun_desc { 
  char *name; 
  virus* (*fun)(virus* v, FILE* output); 
  };
  
int main(int argc, char** argv) {
    FILE* outFile = stdout;
    FILE* inFile = NULL;
    virus* v = NULL;
    
//from task a:
    // FILE* outFile = stdout;
    if (argc != 2) {
        printf("error, exiting program\n");
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    parseFile(fp);

    // virus* v = readVirus(fp);
    // outFile=fopen("test","w");
    //             if (outFile == NULL)
    //                 fprintf(stderr, "Error: could not open output file\nExiting program!\n\n");     
    // if(v!=NULL) printVirus(v,outFile);

//   int menu_input;
//   struct fun_desc menu[] = { 
//     { "Load signatures", readVirus }, 
//     { "Print signatures", printVirus }, 
//     { "Detect viruses", detect }, 
//     { "Fix file", fix }, 
//     { "Quit", quit }, 
//     { NULL, NULL }
//   };
//   int bound = sizeof(menu)/8-2; //lower bound - 1,upper bound - 6
// //   // printf ("%i\n",bound);
//   int flag = 1;   //all flags to deal with double print of menu because of 0A garbage values
  
//   while (1) {
//     /* print menu iteritavly:*/
//     if (flag !=0){
//       printf("Select operation from the following menu:\n");
//       for (int i = 0; i < sizeof(menu)/8-1; i++)
//       {
//         printf( "%i)  %s\n", i,menu[i].name);
//       }
//     }
//     flag = 1; 

//     // /* recieve user input:*/
//     char input[256];
//     if (fgets(input, 256, stdin) == NULL) {
//       break; // if EOF - exit while loop
//     }
//     menu_input = input[0];
//     printf("input is:%i\n",(int)menu_input);
//     if (menu_input==10) flag = 0;   //ascii - hxa garbage 0A
//     // if (menu_input==EOF) break;

//     menu_input=menu_input-48;
//     if (menu_input>=0&&menu_input<=bound)
//     { 
//       printf ("within bounds!\n");
//       /* code */
//     }else {
//       printf("not within bounds!\n");
//       break;
//     }
    
//   /* activate chosen input:*/
    
//     if (menu_input==0)  //load
//     {
//         char anotherInput[256];
//         char* fileName = fgets(anotherInput, 256, stdin);
//         fileName[strcspn(fileName, "\n")] = '\0'; // remove the newline character
//         FILE* fp = fopen(fileName, "r");
//         printf("fp is: %s\n", fp);
//         if (!fp) {
//             printf("Could not open file %s\n", fileName);
//             return 1;
//         }
//         parseFile(fp);
//         // v = menu[0].fun(NULL,fp);
//         printf("\tDEBUG!!!\n");         //@@@@@DEBUG@@@@
//     }
//     if (menu_input==1)  //print
//     {
//         if (v==NULL) break;
//         menu[2].fun(v,outFile);
//     }
//     if (menu_input==2)  //print
//     {
//         printf("not implemented");
//         // menu[3].fun(v,outFile);
//     }
//     if (menu_input==3)  //print
//     {
//         printf("not implemented");
//         // menu[3].fun(v,outFile);
//     }
//     if (menu_input==4)  //print
//     {
//         printf("not implemented");
//         // menu[3].fun(v,outFile);
//     }
//   }

    return 0;
}