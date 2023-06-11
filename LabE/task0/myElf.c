#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

# define buffLen 100
typedef struct {
  char *name;
  void (*fun)();
}fun_desc;
int menu_size = 6;
int debug = 0;
int Currentfd = -1;
Elf32_Ehdr *header; 
char* currentFilenameOpen=NULL;
void* map_start; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to  the size of the file */

int LoadFile(){
    char filename[buffLen];
    int fd;

    fscanf(stdin,"%s",filename);
    if((fd = open(filename, O_RDWR)) < 0) {
      perror("error in open");
      exit(-1);
   }
    if(fstat(fd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }
    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-4);
   }
    if(Currentfd!=-1)
        close(Currentfd);
    Currentfd=fd;
	strcpy((char*)&currentFilenameOpen,(char*)filename);
    return Currentfd;
}

void toggleDebugMode(){
    if (debug == 0) {
        printf("Debug flag now on\n");
        debug = 1;
  }
  else {
    printf("Debug flag now off\n");
    debug = 0;
  }
}
char* dataType(Elf32_Ehdr* header){
    switch (header->e_ident[5]){
    case ELFDATANONE:
        return "invalid data encoding";
        break;
    case ELFDATA2LSB:
        return "2's complement, little endian";
        break;
    case ELFDATA2MSB:
        return "2's complement, big endian";
        break;
    default:
        return "NO DATA";
        break;
    }
}
void examineFile(){
    printf("Enter file name: ");
    if(LoadFile()==-1)
        exit(1);
    header = (Elf32_Ehdr *) map_start;
    if(strncmp((char*)header->e_ident,(char*)ELFMAG, 4)==0){ //is elf file?
        printf("Magic:\t\t\t\t %X %X %X\n", header->e_ident[EI_MAG0],header->e_ident[EI_MAG1],header->e_ident[EI_MAG2]);
        printf("Data:\t\t\t\t %s\n",dataType(header));
        printf("Enty point address:\t\t 0x%x\n",header->e_entry);
        printf("Start of section headers:\t %d (bytes into file)\n",header->e_shoff);
        printf("Number of section headers:\t %d\n",  header->e_shnum);
        printf("Size of section headers:\t %d (bytes)\n",header->e_shentsize);
        printf("Start of program headers:\t %d (bytes into file)\n",header->e_phoff);
        printf("Number of program headers:\t %d\n",header->e_phnum);
        printf("Size of program headers:\t %d (bytes)\n",header->e_phentsize);
   }
    else{
        printf("This is not ELF file\n");
        munmap(map_start, fd_stat.st_size); 
        close(Currentfd); 
        Currentfd = -1;
        currentFilenameOpen = NULL;
    }
}
void printSectionNames(){
    printf("not implemented");
}

void printSymbols(){
    printf("not implemented");
}
void relocationTables(){
    printf("not implemented");
}
void quit(){
    if (debug) { printf("quitting\n");}
    exit(0);
}

void printMenu (fun_desc menu[]){
	// if(currentFilenameOpen!=NULL /*strcmp(currentFilenameOpen,"")!=0*/){
	//     fprintf(stdout,"Current File Open: %s\n",&currentFilenameOpen);
	// }
    printf("Please choose a function:\n");
    for(int i = 0; i < menu_size; i++)
        printf("%d) %s\n", i, menu[i].name);
    printf("Option: ");
}

int getOption (){
  int op;
  scanf("%d", &op);
  if (op >= 0 && op < menu_size){
    printf("\nwhithin bounds \n" );
    return op;
  }
  else{
    printf("\nNot within bounds\n" );
    return -1;
  }
}

int main(int argc, char **argv){
  fun_desc menu[] = {{"Toggle Debug Mode",toggleDebugMode},{"Examine ELF File",examineFile},
                    {"Print Section Names",printSectionNames},{"Print Symbols",printSymbols},
                    {"Relocation Tables",relocationTables},{"Quit",quit}};
  size_t index=0;
  while (menu[index].name != NULL){index = index +1; }
  while (1) {
    printMenu(menu);
    int option = getOption();
    if (option != -1) { menu[option].fun(); }
    printf("\n");
  }
return 0;
}