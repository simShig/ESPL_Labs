#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short sigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

// Reads the virus from the given file pointer
virus* readVirus(FILE* inFile) {
    int nameSize = 16;
    virus* v = (virus*) malloc(sizeof(virus));
    v->sigSize=0;
    fread(&v->sigSize, sizeof(unsigned short), 1, inFile);  //read sigSize
    fread(v->virusName, sizeof(char), nameSize, inFile); //read virName
    v->virusName[nameSize] = '\0';
    int sigLength =0;
    sigLength =v->sigSize;
        if(sigLength==0) return NULL;
    v->sig = (unsigned char*) malloc(sigLength * sizeof(unsigned char));
    fread(v->sig, sizeof(unsigned char), sigLength, inFile); //read signature itself
    
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
        printVirus(virus_list->vir,outFile); 
        virus_list = virus_list->nextVirus; //Move to the next link
    }
}

link* list_append(link* virus_list, virus* data) {
    link* new_link = (link*) malloc(sizeof(link));
    new_link->nextVirus = NULL;
    new_link->vir = data; //Set the data of the new link
    if (virus_list == NULL) { //case the list is empty
        return new_link;
    }
    link* current_link = virus_list;
    while (current_link->nextVirus != NULL) { //go to end of list
        current_link = current_link->nextVirus;
    }
    current_link->nextVirus = new_link; 
    return virus_list;          //return the beggining of the list
}

void list_free(link *virus_list) {
    link *current_link = virus_list;
    while (current_link != NULL) {
        link *temp_link = current_link; //so i can free space without "losing" my pointer to next
        current_link = current_link->nextVirus;
        printf("\tfreed sig of virus %s\n",temp_link->vir->virusName);
        free(temp_link->vir->sig); //free malloc for SIG
        free(temp_link->vir); //free malloc for V
        free(temp_link); //free malloc for link
    }
}

int parseFile(FILE* inFile,link* virus_list){
    int magicNumSize = 4;
    char magicNum[magicNumSize+1];
    // Read the magic number from the file
    fread(magicNum, sizeof(char), magicNumSize, inFile);
    magicNum[magicNumSize] = '\0';
    if(strcmp("VISL", magicNum) != 0){
        printf("magicNum is: %s, exiting program\n",magicNum);
        return -1;  //wrong file prefix
    }
    virus* v=NULL;
    do{
        v = readVirus(inFile);
        if(v!=NULL){
            list_append(virus_list,v);
        }
    }while(v!=NULL);
    printf("\tparsed ok\n");
    return 1;//finished sucssessfully
}

/* ~~~~~~DETECTION STUFF:*/
void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    
    while (virus_list!=NULL)
    {
        virus* current_virus = virus_list->vir;
        for (unsigned int i = 0; i <= size - current_virus->sigSize; i++) {
            if (memcmp(current_virus->sig, buffer + i, current_virus->sigSize) == 0) {
                printf("Virus detected: %s\n", current_virus->virusName);
                printf("Starting byte location in the suspected file: %u\n", i);
                printf("Size of the virus signature: %u\n", current_virus->sigSize);
            }
    
        }
        virus_list=virus_list->nextVirus;
    }
     printf("finished scanning file\n");
   
}

/* FIXING STUFF:*/

void neutralize_virus(char *fileName, int signatureOffset) {
    FILE *file = fopen(fileName, "r+b");
    if (file == NULL) {
        printf("Error: failed to open file '%s'\n", fileName);
        return;
    }
    // Find the size of the file
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileBuffer = (char *) malloc(fileSize);
    fread(fileBuffer, 1, fileSize, file);

    char retInstruction = 0xC3; // RET opcode
    fseek(file, signatureOffset, SEEK_SET);
    fwrite(&retInstruction, 1, 1, file);
    free(fileBuffer);
    fclose(file);
    printf("finished fixing file\n");
}


/* ~~~~Menu stuff:*/
struct fun_desc { 
  char *name; 
  virus* (*fun)(virus* v, FILE* output); 
  };
  

  struct fun_desc menu[] = { 
    { NULL, NULL },
    { "Load signatures", NULL }, 
    { "Print signatures", NULL }, 
    { "Detect viruses", NULL }, 
    { "Fix file", NULL }, 
    { "Quit", NULL }
  };


int main(int argc, char** argv) {
    FILE* outFile = stdout;
    link* virus_list=NULL;
    char buffer[10000];
    size_t bytes_read;
    if (argc != 2) {
        printf("error-wrong amount of command-line arguments,\nexiting program\n");
        return 1;
    }

    FILE* inFile = fopen(argv[1], "r");
    if (!inFile) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    int menu_input;
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

    /* recieve user input:*/
    
    char input[256];
    if (fgets(input, 256, stdin) == NULL) {
      break; // if EOF - exit while loop
    }
    menu_input = input[0];
    if (menu_input==10) flag = 0;   //ascii - hxa garbage 0A

    menu_input=menu_input-48;
    if (menu_input>0&&menu_input<=5)
    { 
    //   printf ("within bounds!\n");       //redundent...
    }else {
      printf("not within bounds!\n");
      break;
    }

   
  /* activate chosen input:*/
    
    if (menu_input==1)  //load
    {
        printf("insert filename:\n");
        char anotherInput[256];
        char* fileName = fgets(anotherInput, 256, stdin);
        fileName[strcspn(fileName, "\n")] = '\0'; // remove the newline character
        FILE* fp = fopen(fileName, "r");
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
        }
        if (virus_list==NULL) continue; //if print before load
        list_print(virus_list,outFile);
        outFile = stdout;       //back to deafult  

    }
    if (menu_input==3)  //detect
    {
            FILE* inFile = fopen(argv[1], "r");

        if (virus_list==NULL){
            printf("no Signatures List has been loaded...\n");
            continue; //if detect before load
        }
        bytes_read = fread(buffer, 1, 10000, inFile);
        printf("!!DEBUG: bytes read: %d from file %d\n",bytes_read,inFile);         //DEBUG

        detect_virus(buffer, bytes_read, virus_list);
        printf("!!DEBUG: finished detecting\n");        //DEBUG
    // if (!inFile) fclose(inFile);
    
    }
    if (menu_input==4)  //fix
    {
        printf("insert sigOffset for fixing:\n");
        int sigOffset;
        char input[10];
        fgets(input, 10, stdin);
        sigOffset = atoi(input);
        printf("sigOff is: %i\n", sigOffset);   
        neutralize_virus(argv[1],sigOffset);
    }
    if (menu_input==5)  //quit
    {
       
        list_free(virus_list);
        printf("quiting\n");
        break;
    }
  

  }
    // fclose(inFile);
    if (!outFile) fclose(outFile);
    return 0;
  }
