#define _FILE_OFFSET_BITS 64    //refferance : https://stackoverflow.com/questions/13893580/calling-stat-from-sys-stat-h-faills-with-value-too-large-for-defined-data-typ
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

int FirstFd = -1;
int SecondFd = -1;

Elf32_Ehdr *header; 
char* firstFilenameOpen=NULL;
char* secondFilenameOpen=NULL;

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
        return SecondFd;

   }else{
    printf("two files are already loaded\n");
    return -1;
   }

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
    if (FirstFd==-1){               //first file examine
        printf("Enter first file name: ");
        if(LoadFile(1)==-1)
            exit(1);
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
        if(LoadFile(2)==-1)
            exit(1);
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
		if(debug){
			fprintf(stderr,"section table address: %p\n",sections_table);
			fprintf(stderr,"string table entry: %p\n",string_table_entry);
            printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\t\toffset(bytes)\n");
        }
		else{printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\n");}
		fprintf(stderr,"File %s :\n",firstFilenameOpen);
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
		if(debug){
			fprintf(stderr,"section table address: %p\n",sections_table);
			fprintf(stderr,"string table entry: %p\n",string_table_entry);
            printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\t\toffset(bytes)\n");
        }
		else{printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\n");}
		fprintf(stderr,"File %s :\n",secondFilenameOpen);
    	for (size_t i = 0; i < header->e_shnum; i++){       
    		Elf32_Shdr* entry = mapLocationSecond+header->e_shoff+(i* header->e_shentsize);     //header->e_shoff+(i* header->e_shentsize) ==> section
        	char* name = mapLocationSecond + string_table_entry->sh_offset + entry->sh_name;
        	printSectionEntry(i,name,entry,header->e_shoff+(i* header->e_shentsize));
    	}
  	}
}

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
}

void printRelTable(char* table_name, Elf32_Shdr* table_entry){
    Elf32_Shdr *dynsym = getTable(".dynsym");
    Elf32_Shdr *dynstr = getTable(".dynstr");

    printf("Relocation section '%s' at offset %#x contains %d entries:\n",
            table_name,table_entry->sh_offset,table_entry->sh_size/sizeof(Elf32_Rel));
    printf(" Offset\t\t  Info\t   Type\t   Sym.Value\tSym.Name\n");
    for(int i=0; i<table_entry->sh_size/sizeof(Elf32_Rel);i++){
        Elf32_Rel* rel_entry = mapLocationFirst+table_entry->sh_offset+(i*sizeof(Elf32_Rel));
        int type = ELF32_R_TYPE(rel_entry->r_info);
        int symbol_table_index = ELF32_R_SYM(rel_entry->r_info);
        Elf32_Sym* symbol_entry = mapLocationFirst+dynsym->sh_offset+(symbol_table_index*sizeof(Elf32_Sym));
        int value = symbol_entry->st_value;
        char* symbol_name = mapLocationFirst+dynstr->sh_offset+symbol_entry->st_name;
        printf("%08x\t%08x    %d\t   %08x\t%s\n",
            rel_entry->r_offset, rel_entry->r_info,type,value,symbol_name);
    }
    printf("\n");
}

void relocationTables(){
    if(FirstFd == -1) {
        perror("No file open!");
    }else {
        Elf32_Shdr *rel_dyn_entry = getTable(".rel.dyn");
        Elf32_Shdr *rel_plt_entry = getTable(".rel.plt");
        
        if (rel_dyn_entry != NULL) { printRelTable(".rel.dyn", rel_dyn_entry); }
        if (rel_plt_entry != NULL) { printRelTable(".rel.plt", rel_plt_entry); }
    } 
}

void quit(){
    if (debug) { printf("quitting\n");}
    exit(0);
}

void printMenu (fun_desc menu[]){
	// if(firstFilenameOpen!=NULL /*strcmp(firstFilenameOpen,"")!=0*/){
	//     fprintf(stdout,"Current File Open: %s\n",&firstFilenameOpen);
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