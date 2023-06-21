#define _FILE_OFFSET_BITS 64    //refferance : https://stackoverflow.com/questions/13893580/calling-stat-from-sys-stat-h-faills-with-value-too-large-for-defined-data-typ
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
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

// Structure to represent a symbol
typedef struct {
    char* name;
    bool is_undefined;
    bool is_defined;
    unsigned int st_name;
    unsigned char st_info;
    unsigned char st_other;
    unsigned short st_shndx;
    unsigned long st_value;
    unsigned long st_size;
} Symbol;

// Structure to represent a symbol table
typedef struct {
    Symbol* symbols;
    int num_symbols;
} SymbolTable;


// Structure to represent the ELF header
typedef struct {
    // Fields needed for the merged ELF file
    char e_ident[16];
    unsigned short e_type;
    unsigned short e_machine;
    unsigned int e_version;
    unsigned long e_entry;
    unsigned long e_phoff;
    unsigned long e_shoff;
    unsigned int e_flags;
    unsigned short e_ehsize;
    unsigned short e_phentsize;
    unsigned short e_phnum;
    unsigned short e_shentsize;
    unsigned short e_shnum;
    unsigned short e_shstrndx;
} ElfHeader;

// Structure to represent a section header
typedef struct {
    unsigned int sh_name;
    unsigned int sh_type;
    unsigned long sh_flags;
    unsigned long sh_addr;
    unsigned long sh_offset;
    unsigned long sh_size;
    unsigned int sh_link;
    unsigned int sh_info;
    unsigned long sh_addralign;
    unsigned long sh_entsize;
} SectionHeader;


int menu_size = 7;
int debug = 0;

int FirstFd = -1;
int SecondFd = -1;

Elf32_Ehdr *header; 
char* firstFilenameOpen=NULL;
char* secondFilenameOpen=NULL;
char filename1[buffLen];
char filename2[buffLen];

void* mapLocationFirst; /* will point to the location of the memory mapped file */
void* mapLocationSecond; /* will point to the location of the memory mapped file */

struct stat fd_stat1; /* this is needed to  the size of the file */
struct stat fd_stat2; /* this is needed to  the size of the file */


Elf32_Shdr* getTable(char* _name){
    Elf32_Shdr* string_table_entry = mapLocationFirst+ header->e_shoff+(header->e_shstrndx*header->e_shentsize);
    for (size_t i = 0; i < header->e_shnum; i++){
        Elf32_Shdr* entry = mapLocationFirst+header->e_shoff+(i* header->e_shentsize);
        char* name = mapLocationFirst + string_table_entry->sh_offset + entry->sh_name;
        if(strcmp(_name, name)==0){
            return entry;
        }
    }
    return NULL;
}

int LoadFile(int fileNum){ 
    char filename[buffLen];
    int fd;

    fscanf(stdin,"%s",filename);
    if((fd = open(filename, O_RDWR)) < 0) {
      perror("error in open");
      exit(-1);
   }
   if(fileNum==1){ 

        if(fstat(fd, &fd_stat1) != 0 ) {
        perror("stat failed");
        exit(-1);
    }
        if ((mapLocationFirst = mmap(0, fd_stat1.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        exit(-4);
    }
        if(FirstFd!=-1)
            close(FirstFd);
        FirstFd=fd;
        strcpy((char*)&firstFilenameOpen,(char*)filename);
        strcpy(filename1, filename);
        return FirstFd;

   }
    if(fileNum==2){
    
        if(fstat(fd, &fd_stat2) != 0 ) {
        perror("stat failed");
        exit(-1);
    }
        if ((mapLocationSecond = mmap(0, fd_stat2.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        exit(-4);
    }
        if(SecondFd!=-1)
            close(SecondFd);
        SecondFd=fd;
        strcpy((char*)&secondFilenameOpen,(char*)filename);
        strcpy(filename2, filename);
        return SecondFd;

   }else{
    printf("two files are already loaded\n");
    return -1;
   }

}
//DEBUGMODE:

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

//EXAMINE ELF:

void examineFile(){
    if (FirstFd==-1){               //first file examine
        printf("Enter first file name: ");
        if(LoadFile(1)==-1){
            exit(1);
        }
        header = (Elf32_Ehdr *) mapLocationFirst;
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
            munmap(mapLocationFirst, fd_stat1.st_size); 
            close(FirstFd); 
            FirstFd = -1;
            firstFilenameOpen = NULL;
        }
        return;
    }
    if (SecondFd==-1){               //first file examine
        printf("Enter Second file name: ");
        if(LoadFile(2)==-1){
            exit(1);
        }
        header = (Elf32_Ehdr *) mapLocationSecond;
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
            munmap(mapLocationSecond, fd_stat2.st_size); 
            close(SecondFd); 
            SecondFd = -1;
            secondFilenameOpen = NULL;
        }
    return;
    }
    //case two already opened:
    printf("Error - failed open atempt: there are two opened files already\n");
    return;
}
//PRINT SECTIONS:

char* sectionType(int value) {
    switch (value) {
        case SHT_NULL:return "NULL";
        case SHT_PROGBITS:return "PROGBITS";
        case SHT_SYMTAB:return "SYMTAB";
        case SHT_STRTAB:return "STRTAB";
        case SHT_RELA:return "RELA";
        case SHT_HASH:return "HASH";
        case SHT_DYNAMIC:return "DYNAMIC";
        case SHT_NOTE:return "NOTE";
        case SHT_NOBITS:return "NOBITS";
        case SHT_REL:return "REL";
        case SHT_SHLIB:return "SHLIB";
        case SHT_DYNSYM:return "DYNSYM";
        default:return "Unknown";
    }
}

void printSectionEntry(int index,char* name ,Elf32_Shdr* section,int offset){
    if(debug){
        printf("[%2d] %-18.18s\t%#06x\t%06d\t%06d\t%-13.10s\t%d\n",index, name ,section->sh_addr,section->sh_offset, section->sh_size, sectionType(section->sh_type),offset );
    }
    else{
        printf("[%2d] %-18.18s\t%#06x\t%06d\t%06d\t%-13.10s\n",index, name ,section->sh_addr,section->sh_offset, section->sh_size, sectionType(section->sh_type) );}
}

void printSectionNames(){
    if(FirstFd ==-1)
        perror("No file is currently open\n");
    else{
    	Elf32_Shdr* sections_table = mapLocationFirst+header->e_shoff;
    	Elf32_Shdr* string_table_entry = mapLocationFirst+header->e_shoff+(header->e_shstrndx*header->e_shentsize); //to get the names
		fprintf(stderr,"File %s :\n",filename1);        //THE PROBLEMMMMM
		if(debug){
			fprintf(stderr,"section table address: %p\n",sections_table);
			fprintf(stderr,"string table entry: %p\n",string_table_entry);
            printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\t\toffset(bytes)\n");
        }
		else{printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\n");}
        for (size_t i = 0; i < header->e_shnum; i++){       
    		Elf32_Shdr* entry = mapLocationFirst+header->e_shoff+(i* header->e_shentsize);     //header->e_shoff+(i* header->e_shentsize) ==> section
        	char* name = mapLocationFirst + string_table_entry->sh_offset + entry->sh_name;
        	printSectionEntry(i,name,entry,header->e_shoff+(i* header->e_shentsize));
    	}
  	}
    //second:
    if(SecondFd !=-1){
    	Elf32_Shdr* sections_table = mapLocationSecond+header->e_shoff;
    	Elf32_Shdr* string_table_entry = mapLocationSecond+header->e_shoff+(header->e_shstrndx*header->e_shentsize); //to get the names
		fprintf(stderr,"File %s :\n",filename2);        //THE PROBLEMMMMM
		if(debug){
			fprintf(stderr,"section table address: %p\n",sections_table);
			fprintf(stderr,"string table entry: %p\n",string_table_entry);
            printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\t\toffset(bytes)\n");
        }
		else{printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\n");}
    	for (size_t i = 0; i < header->e_shnum; i++){       
    		Elf32_Shdr* entry = mapLocationSecond+header->e_shoff+(i* header->e_shentsize);     //header->e_shoff+(i* header->e_shentsize) ==> section
        	char* name = mapLocationSecond + string_table_entry->sh_offset + entry->sh_name;
        	printSectionEntry(i,name,entry,header->e_shoff+(i* header->e_shentsize));
    	}
  	}
}

//PRINT SYMBOLSS:

void printSymbols(){
    if (FirstFd == -1){
        perror("No file open!");
    }
    else{
        Elf32_Shdr *symbol_table_entry = getTable(".symtab");
        Elf32_Shdr *strtab = getTable(".strtab");       //get the entry of "name" --> symbol name
        Elf32_Shdr *shstrtab = getTable(".shstrtab");   // --> section name
        if (symbol_table_entry == NULL){
            perror("Symbol table not found!");
        }
        else {
            int entry_num = symbol_table_entry->sh_size / sizeof(Elf32_Sym);
            fprintf(stderr,"File %s :\n",filename1);  ///filename

            if(debug){
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\t\tsize\n");
            }
            else{
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\n");
            }
            for (int i = 0; i < entry_num; i++)
            {
                Elf32_Sym *symb_entry = mapLocationFirst + symbol_table_entry->sh_offset + (i * sizeof(Elf32_Sym));
                char *section_name;
                if (symb_entry->st_shndx == 0xFFF1) { section_name = "ABS";}
                else if (symb_entry->st_shndx == 0x0) { section_name = "UND";}
                else {
                    Elf32_Shdr *section_entry = mapLocationFirst + header->e_shoff + (symb_entry->st_shndx * header->e_shentsize);
                    section_name = mapLocationFirst + shstrtab->sh_offset + section_entry->sh_name;
                }

                char *symb_name = mapLocationFirst + strtab->sh_offset + symb_entry->st_name;
                char *symb_size = mapLocationFirst + strtab->sh_offset + symb_entry->st_size;

                if (debug){
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t\%-20.30s\t\t%-20.30s\n",
                    i, symb_entry->st_value, symb_entry->st_shndx, section_name,symb_name,symb_size);
                }
                else {
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t\%-20.30s\n",
                        i, symb_entry->st_value, symb_entry->st_shndx, section_name,symb_name);
                }
            }
            
        }
    }
    //second:
    if (SecondFd!=-1){
        Elf32_Shdr *symbol_table_entry = getTable(".symtab");
        Elf32_Shdr *strtab = getTable(".strtab");       //get the entry of "name" --> symbol name
        Elf32_Shdr *shstrtab = getTable(".shstrtab");   // --> section name
        if (symbol_table_entry == NULL){
            perror("Symbol table not found!");
        }
        else {
            int entry_num = symbol_table_entry->sh_size / sizeof(Elf32_Sym);
            fprintf(stderr,"File %s :\n",filename2);  ///filename

            if(debug){
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\t\tsize\n");
            }
            else{
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\n");
            }
            for (int i = 0; i < entry_num; i++)
            {
                Elf32_Sym *symb_entry = mapLocationSecond + symbol_table_entry->sh_offset + (i * sizeof(Elf32_Sym));
                char *section_name;
                if (symb_entry->st_shndx == 0xFFF1) { section_name = "ABS";}
                else if (symb_entry->st_shndx == 0x0) { section_name = "UND";}
                else {
                    Elf32_Shdr *section_entry = mapLocationSecond + header->e_shoff + (symb_entry->st_shndx * header->e_shentsize);
                    section_name = mapLocationSecond + shstrtab->sh_offset + section_entry->sh_name;
                }

                char *symb_name = mapLocationSecond + strtab->sh_offset + symb_entry->st_name;
                char *symb_size = mapLocationSecond + strtab->sh_offset + symb_entry->st_size;

                if (debug){
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t\%-20.30s\t\t%-20.30s\n",
                    i, symb_entry->st_value, symb_entry->st_shndx, section_name,symb_name,symb_size);
                }
                else {
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t\%-20.30s\n",
                        i, symb_entry->st_value, symb_entry->st_shndx, section_name,symb_name);
                }
            }
            
        }
    }

}

// QUIT:

void quit(){ //eden-should close whats open and wxit normally
    if (debug) { printf("quitting\n");}
    if (FirstFd!=-1) close(FirstFd);
    if (SecondFd!=-1) close(SecondFd);
    exit(0);
}


//CHECK BEFORE MERGE:

Elf32_Sym* get_symbol(Elf32_Sym* symbols, char* strtab, int symbols_size, char* name){
  if(strcmp(name, "") == 0){
    return NULL;
  }
  for(int i = 1; i < symbols_size; i++){
    if(strcmp(name, &strtab[symbols[i].st_name]) == 0){
      return symbols + i;
    }
  }
  return NULL;
}

Elf32_Shdr* extractSection(Elf32_Shdr* sections, char* shstrtab, int sections_size, char* name){
  for(int i = 1; i < sections_size; i++){
    if(strcmp(name, &shstrtab[sections[i].sh_name]) == 0){
      return sections + i;
    }
  }
  return NULL;
}

int fieldsArrange(void* map_start, Elf32_Ehdr** header, Elf32_Shdr** shdr, Elf32_Sym** symbols, char** shstrtab, char** strtab, int* symbol_size){
  int counter = 0;
  *header = (Elf32_Ehdr*) map_start;
  *shdr = (Elf32_Shdr*) (map_start + (*header)->e_shoff);
  *shstrtab = (char*) (map_start + (*shdr)[(*header)->e_shstrndx].sh_offset);
  for(int i = 0; i < (*header)->e_shnum; i++){
    if((*shdr)[i].sh_type == SHT_SYMTAB || (*shdr)[i].sh_type == SHT_DYNSYM){
      counter++;
      *symbols = (Elf32_Sym*) (map_start + (*shdr)[i].sh_offset);
      *strtab = (char*)(map_start + (*shdr)[(*shdr)[i].sh_link].sh_offset);
      *symbol_size = (*shdr)[i].sh_size / (*shdr)[i].sh_entsize;  
    }
  }
  if (counter==1) return 1; //true
  else return 0;    //false
//   return (counter == 1);
}

void CheckMerge(){
  //there are two files
  if(FirstFd==-1||SecondFd==-1){
    printf("there are no two files to merge\n");
    return;
  }
  int symbol_size1, symbol_size2;
  char* shstrtab1, *shstrtab2;
  char* strtab1, *strtab2;
  Elf32_Ehdr* header1, *header2;
  Elf32_Sym* symbols1, *symbols2;
  Elf32_Shdr* shdr1, *shdr2;
  
  if(fieldsArrange(mapLocationFirst, &header1, &shdr1, &symbols1, &shstrtab1, &strtab1, &symbol_size1) == 0){
    printf("feature not supported\n");
    return;
  }
  if(fieldsArrange(mapLocationSecond, &header2, &shdr2, &symbols2, &shstrtab2, &strtab2, &symbol_size2) == 0){
    printf("feature not supported\n");
    return;
  }
printf("File %s :\n",filename1);        //filename header

  for(int i = 1; i < symbol_size1; i++){
    Elf32_Sym* symbol = get_symbol(symbols2, strtab2, symbol_size2, &strtab1[symbols1[i].st_name]);
    if(symbols1[i].st_shndx == SHN_UNDEF){
      if(symbol == NULL || symbol->st_shndx == SHN_UNDEF){
        printf("Sym %s is undefined\n", &strtab1[symbols1[i].st_name]);
      }
    } else {
      if(symbol != NULL && symbol->st_shndx != SHN_UNDEF){
        printf("Sym %s is multiply defined\n", &strtab1[symbols1[i].st_name]);
      }
    }
  }
  
  printf("\nFile %s :\n",filename2);        //filename header

  for(int i = 1; i < symbol_size2; i++){
    Elf32_Sym* symbol = get_symbol(symbols1, strtab1, symbol_size1, &strtab2[symbols2[i].st_name]);
    if(symbols2[i].st_shndx == SHN_UNDEF){
      if(symbol == NULL || symbol->st_shndx == SHN_UNDEF){
        printf("Sym %s is undefined\n", &strtab2[symbols2[i].st_name]);
      }
    } else {
      if(symbol != NULL && symbol->st_shndx != SHN_UNDEF){
        printf("Sym %s is multiply defined\n", &strtab2[symbols2[i].st_name]);
      }
    }
  }
}




//MERGE:

void MergeElfFiles(){
  //there are two files
  if(FirstFd==-1||SecondFd==-1){
    printf("there are no two files for merging\n");
    return;
  }
  Elf32_Ehdr* header1, *header2;
  Elf32_Shdr* shdr1, *shdr2;
  Elf32_Sym* symbols1, *symbols2;
  char* shstrtab1, *shstrtab2;
  char* strtab1, *strtab2;
  int symbol_size1, symbol_size2;
  
  if(fieldsArrange(mapLocationFirst, &header1, &shdr1, &symbols1, &shstrtab1, &strtab1, &symbol_size1) == 0){
    printf("Error: Format not supported!\n");
    return;
  }
  if(fieldsArrange(mapLocationSecond, &header2, &shdr2, &symbols2, &shstrtab2, &strtab2, &symbol_size2) == 0){
    printf("Error: Format not supported!\n");
    return;
  }

  FILE* file = fopen("out.ro", "wb");
  fwrite((char*) header1, 1, header1->e_ehsize, file);
  Elf32_Shdr copy_shdr[header1->e_shnum];
  memcpy((char*) copy_shdr, (char*)shdr1, header1->e_shnum * header1->e_shentsize);
  for(int i = 1; i < header1->e_shnum; i++){
    copy_shdr[i].sh_offset = ftell(file);
    if(strcmp(&shstrtab1[shdr1[i].sh_name], ".text") == 0 || strcmp(&shstrtab1[shdr1[i].sh_name], ".data") == 0 || strcmp(&shstrtab1[shdr1[i].sh_name], ".rodata") == 0){
      fwrite((char*)(mapLocationFirst + shdr1[i].sh_offset), 1, shdr1[i].sh_size, file);
      Elf32_Shdr* section = extractSection(shdr2, shstrtab2, header2->e_shnum, &shstrtab1[shdr1[i].sh_name]);
      if(section != NULL){
        fwrite((char*)(mapLocationSecond + section->sh_offset), 1, section->sh_size, file);
        copy_shdr[i].sh_size = shdr1[i].sh_size + section->sh_size;
      }
    } else if(strcmp(&shstrtab1[shdr1[i].sh_name], ".symtab") == 0 ){
        Elf32_Sym copy_sym[symbol_size1];
        memcpy((char*) copy_sym, (char*)symbols1, shdr1[i].sh_size);
        for(int j = 1; j < symbol_size1; j++){
          if(symbols1[j].st_shndx == SHN_UNDEF){
            Elf32_Sym* symbol = get_symbol(symbols2, strtab2, symbol_size2, &strtab1[symbols1[j].st_name]);
            copy_sym[j].st_value = symbol->st_value;
            Elf32_Shdr* section = extractSection(shdr1, shstrtab1, header1->e_shnum, &shstrtab2[shdr2[symbol->st_shndx].sh_name]);
            copy_sym[j].st_shndx = section - shdr1;
          }
        }
        fwrite((char*) copy_sym, 1, shdr1[i].sh_size, file);
    } else {
      fwrite((char*)(mapLocationFirst + shdr1[i].sh_offset), 1, shdr1[i].sh_size, file);
    }
  }
  int offset = ftell(file);
  fwrite((char*) copy_shdr, 1, header1->e_shnum * header1->e_shentsize, file);
  fseek(file, 32, SEEK_SET); //32 is the offset for e_shoff
  fwrite((char*) (&offset), 1, sizeof(int), file);
  fclose(file);
}


void printMenu (fun_desc menu[]){

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
                    {"Check Files for Merge", CheckMerge},{"Merge ELF Files", MergeElfFiles},
                    {"Quit",quit}};
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