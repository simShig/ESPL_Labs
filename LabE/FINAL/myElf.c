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




// Function to merge two ELF files
void MergeELF(const char* file1, const char* file2, const char* output) {
    // Open the input ELF files
    FILE* elf1 = fopen(file1, "rb");
    FILE* elf2 = fopen(file2, "rb");

    // Create the output ELF file
    FILE* mergedElf = fopen(output, "wb");

    // Read the ELF header of the first file
    ElfHeader header1;
    fread(&header1, sizeof(ElfHeader), 1, elf1);

    // Write the ELF header to the merged ELF file
    fwrite(&header1, sizeof(ElfHeader), 1, mergedElf);

    // Read the section header table of the first file
    SectionHeader* sectionHeaders1 = (SectionHeader*)malloc(sizeof(SectionHeader) * header1.e_shnum);
    fseek(elf1, header1.e_shoff, SEEK_SET);
    fread(sectionHeaders1, sizeof(SectionHeader), header1.e_shnum, elf1);

    // Loop over the section headers and process each section
    for (int i = 0; i < header1.e_shnum; i++) {
        SectionHeader sectionHeader = sectionHeaders1[i];

        // Concatenate .text, .data, .rodata sections from both files
        if (strcmp(".text", (char*)(sectionHeaders1 + sectionHeader.sh_name)) == 0 ||
            strcmp(".data", (char*)(sectionHeaders1 + sectionHeader.sh_name)) == 0 ||
            strcmp(".rodata", (char*)(sectionHeaders1 + sectionHeader.sh_name)) == 0) {

            // Write the section contents from the first file
            fseek(elf1, sectionHeader.sh_offset, SEEK_SET);
            for (int j = 0; j < sectionHeader.sh_size; j++) {
                fputc(fgetc(elf1), mergedElf);
            }

            // Find the corresponding section in the second file
            SectionHeader sectionHeader2;
            for (int j = 0; j < header1.e_shnum; j++) {
                if (strcmp((char*)(sectionHeaders1 + sectionHeaders1[j].sh_name),
                           (char*)(sectionHeaders1 + sectionHeader.sh_name)) == 0) {
                    sectionHeader2 = sectionHeaders1[j];
                    break;
                }
            }

            // Write the section contents from the second file
            fseek(elf2, sectionHeader2.sh_offset, SEEK_SET);
            for (int j = 0; j < sectionHeader2.sh_size; j++) {
                fputc(fgetc(elf2), mergedElf);
            }

            // Update the section header size
            sectionHeader.sh_size += sectionHeader2.sh_size;
        }

        // Write the modified section header to the merged ELF file
        fseek(mergedElf, header1.e_shoff + (sizeof(SectionHeader) * i), SEEK_SET);
        fwrite(&sectionHeader, sizeof(SectionHeader), 1, mergedElf);
    }

    // Close the input ELF files
    fclose(elf1);
    fclose(elf2);

    // Update the e_shoff field in the ELF header
    fseek(mergedElf, offsetof(ElfHeader, e_shoff), SEEK_SET); ///gpt wrote this line, what do i need to write instead of "offsetof(ElfHeader, e_shoff)"?
    fwrite(&header1.e_shoff, sizeof(unsigned long), 1, mergedElf);

    // Close the merged ELF file
    fclose(mergedElf);

    // Free the allocated memory
    free(sectionHeaders1);
}



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

void quit(){ //eden-should close whats open and wxit normally
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
///eden added:
// Function to check for symbol merge errors
void CheckMerge(SymbolTable SYMTAB1, SymbolTable SYMTAB2) {
    // Check if symbol tables are available
    if (SYMTAB1.symbols == NULL || SYMTAB2.symbols == NULL) {
        printf("Error: ELF files not opened and mapped.\n");
        return;
    }
      // Check if there is exactly one symbol table in each ELF file
    if (SYMTAB1.num_symbols != 1 || SYMTAB2.num_symbols != 1) {
        printf("Feature not supported: Multiple symbol tables found.\n");
        return;
    }
    Symbol* symbols1 = SYMTAB1.symbols;  // Symbols of SYMTAB1
    Symbol* symbols2 = SYMTAB2.symbols;  // Symbols of SYMTAB2

    // Loop over symbols in SYMTAB1
    for (int i = 1; i < SYMTAB1.num_symbols; i++) {
        Symbol sym = symbols1[i];

        // Check if sym is UNDEFINED
        if (sym.is_undefined) {
            bool found_sym = false;
 // Search for sym in SYMTAB2
            for (int j = 1; j < SYMTAB2.num_symbols; j++) {
                if (strcmp(sym.name, symbols2[j].name) == 0) {
                    found_sym = true;

                    // Check if sym is also UNDEFINED in SYMTAB2
                    if (symbols2[j].is_undefined) {
                        printf("Symbol %s undefined\n", sym.name);
                    }
                    break;
                }
            }
             // Check if sym is not found in SYMTAB2
            if (!found_sym) {
                printf("Symbol %s undefined\n", sym.name);
            }
        }
          // Check if sym is defined
        else if (sym.is_defined) {
            // Search for sym in SYMTAB2
            for (int j = 1; j < SYMTAB2.num_symbols; j++) {
                if (strcmp(sym.name, symbols2[j].name) == 0) {
                    // Check if sym is also defined in SYMTAB2
                    if (symbols2[j].is_defined) {
                        printf("Symbol %s multiply defined\n", sym.name);
                    }
                    break;
                }
            }
        }
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