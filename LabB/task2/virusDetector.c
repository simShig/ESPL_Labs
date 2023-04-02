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
    fprintf(output, "Signature:");

    // Print the virus signature in hexadecimal format
    for (int i = 0; i < v->sigSize; i++) {
        if(i%20==0) fprintf(output,"\n");
        fprintf(output, "%02X ", v->sig[i]);
    }
    fprintf(output, "\n\n");
}


/* linked list stuff:*/

typedef struct link link;
typedef struct link {
link *nextVirus;
virus *vir;
}link;


void list_print(link *virus_list, FILE* outFile) {
    while (virus_list != NULL) {
        printVirus(virus_list->vir,outFile);     //change back to outFile
        virus_list = virus_list->nextVirus; /* Move to the next link. */
    }
    fprintf(outFile,"\tDEBUG!!!end of listPrint, outFile is:%s\n",outFile);         //@@@@@DEBUG@@@@

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
        printf("\tinside list_free, current link is:%s\n",current_link->vir->virusName);
        link *temp_link = current_link; /* Store a reference to the current link. */
        current_link = current_link->nextVirus; /* Move to the next link before freeing the current link. */
        free(temp_link->vir->sig); /* Free the memory allocated for the virus struct. */
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
    // printf("\t~DEBUG: magicNum is: %s\n",magicNum); //###DEBUG
    // printf("\t~DEBUG: sizeof(magicNum) is: %i\n",sizeof(magicNum)); //###DEBUG
        /* doesnot compares correctly,need to fix*/
    if(strcmp("VISL", magicNum) != 0){
        printf("magicNum is: %s, exiting program\n",magicNum);
        return -1;  //wrong file prefix
    }
    
    virus* v=NULL;
    do{
        v = readVirus(inFile);
        if(v!=NULL){
            list_append(virus_list,v);
            // printVirus(v,stdout);
            // printf("\n\n");
        }
    }while(v!=NULL);
    printf("parsed ok\n");
    return 1;//finished sucssessfully
}

/* ~~~~~~DETECTION STUFF:*/
void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    // printf("\tDEBUG1 in detect_virus!!!\n");         //@@@@@DEBUG@@@@
    while (virus_list!=NULL)
    {
        virus* current_virus = virus_list->vir;
    // for (virus *current_virus = virus_list->vir; virus_list != NULL; current_virus = ) {
        for (unsigned int i = 0; i <= size - current_virus->sigSize; i++) {
            if (memcmp(current_virus->sig, buffer + i, current_virus->sigSize) == 0) {
                printf("\tDEBUG2 in detect_virus!!!v.name is %s,i=%i,size-sigSize=%i \n",current_virus->virusName,i,size-current_virus->sigSize);         //@@@@@DEBUG@@@@
                printf("Virus detected: %s\n", current_virus->virusName);
                printf("Starting byte location in the suspected file: %u\n", i);
                printf("Size of the virus signature: %u\n", current_virus->sigSize);
            }
    
        }
        virus_list=virus_list->nextVirus;
    }
}

/* FIXING STUFF:*/
void fix_virus(char *buffer, unsigned int size, link *virus_list) {
    // printf("\tDEBUG1 in detect_virus!!!\n");         //@@@@@DEBUG@@@@
    while (virus_list!=NULL)
    {
        virus* current_virus = virus_list->vir;
    // for (virus *current_virus = virus_list->vir; virus_list != NULL; current_virus = ) {
        for (unsigned int i = 0; i <= size - current_virus->sigSize; i++) {
            if (memcmp(current_virus->sig, buffer + i, current_virus->sigSize) == 0) {
                printf("\tDEBUG2 in detect_virus!!!v.name is %s,i=%i,size-sigSize=%i \n",current_virus->virusName,i,size-current_virus->sigSize);         //@@@@@DEBUG@@@@
                // neutralize_virus("infected",107);
                printf("Virus neutrulized?\n");
            }
    
        }
        virus_list=virus_list->nextVirus;
    }
}

void neutralize_virus(char *fileName, int signatureOffset) {
    // Open the file for reading and writing
    FILE *file = fopen(fileName, "r+b");
    if (file == NULL) {
        printf("Error: failed to open file '%s'\n", fileName);
        return;
    }

    // Find the size of the file
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the entire file into memory
    char *fileBuffer = (char *) malloc(fileSize);
    fread(fileBuffer, 1, fileSize, file);
       // Neutralize the virus by overwriting the first byte with a RET instruction
            char retInstruction = 0xC3; // RET instruction opcode
            fseek(file, signatureOffset, SEEK_SET);
            printf("\tDEBUG after seek before write!!!\n");         //@@@@@DEBUG@@@@

            fwrite(&retInstruction, 1, 1, file);

    // Clean up
    free(fileBuffer);
    fclose(file);
}






/* ~~~~Menu stuff:*/
struct fun_desc { 
  char *name; 
  virus* (*fun)(virus* v, FILE* output); 
  };
  

  struct fun_desc menu[] = { 
    { NULL, NULL },
    { "Load signatures", readVirus }, 
    { "Print signatures", printVirus }, 
    { "Detect viruses", NULL }, 
    { "Fix file", NULL }, 
    { "Quit", NULL }
  };


int main(int argc, char** argv) {
    FILE* outFile = stdout;
    link* virus_list=NULL;
    FILE* inFile = NULL;
    char buffer[10000];
    size_t bytes_read;
    if (argc != 1) {
        printf("error, exiting program\n");
        return 1;
    }

    // FILE* fp = fopen(argv[1], "r");
    // if (!fp) {
    //     printf("Could not open file %s\n", argv[1]);
    //     return 1;
    // }
    
    int menu_input;
//   int bound = sizeof(menu)/8-2; //lower bound - 1,upper bound - 6
//   // printf ("%i\n",bound);
  int flag = 1;   //all flags to deal with double print of menu because of 0A garbage values
  
  while (1) {
    /* print menu iteritavly:*/
    if (flag !=0){
      printf("Select operation from the following menu:\n");
      for (int i = 1; i < 6; i++)
      {
        printf( "%i)  %s\n", i,menu[i].name);
      }
    }
    flag = 1; 

    // /* recieve user input:*/
    
    char input[256];
    if (fgets(input, 256, stdin) == NULL) {
      break; // if EOF - exit while loop
    }
    menu_input = input[0];
    // printf("input is:%i\n",menu_input);
    if (menu_input==10) flag = 0;   //ascii - hxa garbage 0A
    // if (menu_input==EOF) break;

    menu_input=menu_input-48;
    if (menu_input>0&&menu_input<=5)
    { 
      printf ("within bounds!\n");
      /* code */
    }else {
      printf("not within bounds!\n");
      break;
    }

   
//   /* activate chosen input:*/
    
    if (menu_input==1)  //load
    {
        printf("insert filename:\n");
        char anotherInput[256];
        char* fileName = fgets(anotherInput, 256, stdin);
        fileName[strcspn(fileName, "\n")] = '\0'; // remove the newline character
        FILE* fp = fopen(fileName, "r");
        printf("fp is: %s\n", fileName);
        if (!fp) {
            printf("Could not open file %s\n", fileName);
            return 1;
        }
        int isParsed=0; //-1 no, +1 yes
        isParsed = parseFile(fp,&virus_list);
        if(isParsed!=1){
            printf("parse failed...\n");
            continue;;   //continue loop if parse fails
        }
        fclose(fp);
    }
    if (menu_input==2){  //print
        printf("insert output filename (enter if default:stdout)\n");
        char anotherInput[256];
        char* fileName = fgets(anotherInput, 256, stdin);
        if (fileName[0]!=10){ //if not "enter"
            fileName[strcspn(fileName, "\n")] = '\0'; // remove the newline character
            outFile = fopen(fileName, "w");
            printf("fp is: %s\n", fileName);   
        }
        if (virus_list==NULL) continue; //if print before load
        list_print(virus_list,outFile);     //BUG - when printing to stdout - prints all. when print to file - prints partly (95%)
    }
    if (menu_input==3)  //detect
    {
        if (virus_list==NULL) continue; //if detect before load
        // FILE* inFile = NULL;
        printf("insert input filename for scanning:\n");
        char anotherInput[256];
        char* fileName = fgets(anotherInput, 256, stdin);
        fileName[strcspn(fileName, "\n")] = '\0'; // remove the newline character
        inFile = fopen(fileName, "r");
        if (fileName[0]==10||inFile==NULL){ //if "enter" || couldnt open file
            printf("couldnt open file\n");
            continue;
        }
        printf("fp is: %s\n", fileName);   
        bytes_read = fread(buffer, 1, 10000, inFile);
        printf("\tDEBUG in menuInput=3!!!\n");         //@@@@@DEBUG@@@@
        detect_virus(buffer, bytes_read, virus_list);
        fclose(inFile);
        // printf("not implemented");
        // menu[3].fun(v,outFile);
    }
    if (menu_input==4)  //fix
    {
        printf("insert input filename for fixing:\n");
        char anotherInput[256];
        char* fileName = fgets(anotherInput, 256, stdin);
        fileName[strcspn(fileName, "\n")] = '\0'; // remove the newline character
        inFile = fopen(fileName, "r");
        if (fileName[0]==10||inFile==NULL){ //if "enter" || couldnt open file
            printf("couldnt open file\n");
            continue;
        }
        printf("insert sigOffset place:\n");
        int sigOffset;
        char input[10];
        fgets(input, 10, stdin);
        sigOffset = atoi(input);
        printf("sigOff is: %i\n", sigOffset);   
        neutralize_virus(fileName,sigOffset);
           printf("\t after neutrulize???\n");
        // menu[3].fun(v,outFile);
        fclose(inFile);
    }
    if (menu_input==5)  //quit
    {
        // free(outFile);
        // printf("outfile freed\n");
        // free(inFile);
        // printf("infile freed\n");
        list_free(virus_list);
        printf("vir_list freed\n");
        // free(buffer);
        // printf("buffer freed\n");
        printf("quiting\n");
        break;
    }
  

  }
    
    
    
    
    
    // int isParsed=0; //-1 no, +1 yes
    // isParsed = parseFile(fp,&virus_list);
    // if(isParsed!=1) return 0;
    // // printf("\t~~DEBUG~~\n");
    // outFile=fopen("newtest","w");
    //             if (outFile == NULL)
    //                 fprintf(stderr, "Error: could not open output file\nExiting program!\n\n");     
        // list_print(virus_list,outFile);
    if (inFile!=NULL) close(inFile);
    if (outFile!=NULL) close(outFile);
    return 0;
  }
