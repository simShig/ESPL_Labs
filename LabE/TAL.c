#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

struct fun_desc{
  char *name;
  void (*fun)();
};

void* map_start1;
int size1;
char file_name1[100];
void* map_start2;
int size2;
char file_name2[100];
int total_files = 0;
int debug =0;
int fd = -1;

void ToggleDebug_Mode(){
    if (debug == 0){
        debug = 1;
        printf("Debug flag now on\n");
    }
    else{
        debug = 0;
        printf("Debug flag now off\n");
    }
}

void Examine_ELF_File(){
  if(total_files == 2){
    printf("You have two opened files already\n");
    return;
  }
  char buffer[100];
  printf("Please enter file name:\n");
  fgets(buffer, 100, stdin);
  buffer[strlen(buffer)-1] = '\0';
  fd = open(buffer, O_RDONLY);
  if(fd < 0){
    perror("open");
    return;
  }
  struct stat sb;
  if(fstat(fd, &sb) < 0){
    perror("fstat");
    close(fd);
    return;
  }
  void* map_start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if(map_start == MAP_FAILED){
    perror("mmap");
    close(fd);
    return;
  }
  Elf32_Ehdr* header = (Elf32_Ehdr*) map_start;
  if(header->e_ident[0] == ELFMAG0 && header->e_ident[1] == ELFMAG1 && 
  header->e_ident[2] == ELFMAG2 && header->e_ident[3] == ELFMAG3){
    printf("Magic: %X %X %X %X\n", header->e_ident[0], header->e_ident[1], header->e_ident[2], header->e_ident[3]);
    if(header->e_ident[EI_DATA] == ELFDATA2LSB){
        printf("Data: 2's complement, little endian\n");
    } else {
        printf("Data: 2's complement, big endian\n");
    }
    printf("Entry point: %X\n", header->e_entry);
    printf("Section header offset: %d\n", header->e_shoff);
    printf("Section header count: %d\n", header->e_shnum);
    printf("Section header size: %d\n", header->e_shentsize);
    printf("Program header offset: %d\n", header->e_phoff);
    printf("Program header count: %d\n", header->e_phnum);
    printf("Program header size: %d\n", header->e_phentsize);
    if(total_files == 0){
      map_start1 = map_start;
      size1 = sb.st_size;
      strcpy(file_name1, buffer);
    } else {
      map_start2 = map_start;
      size2 = sb.st_size;
      strcpy(file_name2, buffer);
    }
    total_files++;
  } else {
    printf("Not an elf file!\n");
  }
}

char* section_type(Elf32_Shdr* shdr){
      if(shdr->sh_type == SHT_NULL) return "NULL";
    if(shdr->sh_type == SHT_PROGBITS) return "PROGBITS";
    if(shdr->sh_type == SHT_SYMTAB) return "SYMTAB";
    if(shdr->sh_type == SHT_STRTAB) return "STRTAB";
    if(shdr->sh_type == SHT_RELA) return "RELA";
    if(shdr->sh_type == SHT_HASH) return "HASH";
    if(shdr->sh_type == SHT_DYNAMIC) return "DYNAMIC";
    if(shdr->sh_type == SHT_NOTE) return "NOTE";
    if(shdr->sh_type == SHT_NOBITS) return "NOBITS";
    if(shdr->sh_type == SHT_REL) return "REL";
    if(shdr->sh_type == SHT_SHLIB) return "SHLIB";
    if(shdr->sh_type == SHT_DYNSYM) return "DYNSYM";
    if(shdr->sh_type == SHT_INIT_ARRAY) return "INIT_ARRAY";
    if(shdr->sh_type == SHT_FINI_ARRAY) return "FINI_ARRAY";
    if(shdr->sh_type == SHT_PREINIT_ARRAY) return "PREINIT_ARRAY";
    if(shdr->sh_type == SHT_GROUP) return "GROUP";
    if(shdr->sh_type == SHT_SYMTAB_SHNDX) return "SYMTAB_SHNDX";
    if(shdr->sh_type == SHT_NUM) return "NUM";
    if(shdr->sh_type == SHT_LOOS) return "LOOS";
    if(shdr->sh_type == SHT_GNU_ATTRIBUTES) return "GNU_ATTRIBUTES";
    if(shdr->sh_type == SHT_GNU_HASH) return "GNU_HASH";
    if(shdr->sh_type == SHT_GNU_LIBLIST) return "GNU_LIBLIST";
    if(shdr->sh_type == SHT_CHECKSUM) return "CHECKSUM";
    if(shdr->sh_type == SHT_LOSUNW) return "LOSUNW";
    if(shdr->sh_type == SHT_SUNW_move) return "SUNW_move";
    if(shdr->sh_type == SHT_SUNW_COMDAT) return "SUNW_COMDAT";
    if(shdr->sh_type == SHT_SUNW_syminfo) return "SUNW_syminfo";
    if(shdr->sh_type == SHT_GNU_verdef) return "GNU_verdef";
    if(shdr->sh_type == SHT_GNU_verneed) return "GNU_verneed";
    if(shdr->sh_type == SHT_GNU_versym) return "GNU_versym";
    if(shdr->sh_type == SHT_HISUNW) return "HISUNW";
    if(shdr->sh_type == SHT_HIOS) return "HIOS";
    if(shdr->sh_type == SHT_LOPROC) return "LOPROC";
    if(shdr->sh_type == SHT_HIPROC) return "HIPROC";
    if(shdr->sh_type == SHT_LOUSER) return "LOUSER";
    if(shdr->sh_type == SHT_HIUSER) return "HIUSER";
    return "";
}

void Print_Sections(void* map_start){
  Elf32_Ehdr* header = (Elf32_Ehdr*) map_start;
  Elf32_Shdr* shdr = (Elf32_Shdr*) (map_start + header->e_shoff);
  char* shstrtab = (char*) (map_start + shdr[header->e_shstrndx].sh_offset);

  if(debug){
    printf("\n DEBUG:\n");
    printf("section header table file offset in bytes is -  %x\n", header->e_shoff);
    printf("shstrndx - Section header string table index - is %d \n",header->e_shstrndx);
  }

  for(int i = 0; i < header->e_shnum; i++){
    //[index] section_name section_address section_offset section_size section_type
    printf("[%02d] %-20s %08x %06x %06x %s\n", i, &shstrtab[shdr[i].sh_name], shdr[i].sh_addr, shdr[i].sh_offset, shdr[i].sh_size, section_type(shdr + i));
  }
}

void Print_Section_Names(){
  if(total_files == 0){
    printf("You need to examine files first!\n");
  } else if(total_files == 1){
    printf("File: %s\n", file_name1);
    Print_Sections(map_start1);
  } else {
    printf("File: %s\n", file_name1);
    Print_Sections(map_start1);
    printf("\n");
    printf("File: %s\n", file_name2);
    Print_Sections(map_start2);
  }
}

void Print_symbols_table(void* map_start){
  Elf32_Ehdr* header = (Elf32_Ehdr*) map_start;
  Elf32_Shdr* shdr = (Elf32_Shdr*) (map_start + header->e_shoff);
  char* shstrtab = (char*) (map_start + shdr[header->e_shstrndx].sh_offset);
  for(int i = 0; i < header->e_shnum; i++){
    if(shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM){
      Elf32_Sym* symbols = (Elf32_Sym*) (map_start + shdr[i].sh_offset);
      char* strtab = (char*)(map_start + shdr[shdr[i].sh_link].sh_offset);
      int size = shdr[i].sh_size / shdr[i].sh_entsize;
      if(debug)
        printf("\nDEBUG: Symbol table '%s' contains %d entries:\n", &shstrtab[shdr[i].sh_name], size);
      for(int j = 0; j < size; j++){
        if(symbols[j].st_shndx == SHN_ABS){
          //[index] value section_index section_name symbol_name
          printf("[%d] %08x ABS %-20s %-20s\n", j, symbols[j].st_value, "", &strtab[symbols[j].st_name]);
        } else if(symbols[j].st_shndx == SHN_UNDEF){
          printf("[%d] %08x UND %-20s %-20s\n", j, symbols[j].st_value, "", &strtab[symbols[j].st_name]);
        } else {
          printf("[%d] %08x %03d %-20s %-20s\n", j, symbols[j].st_value, symbols[j].st_shndx, &shstrtab[shdr[symbols[j].st_shndx].sh_name], &strtab[symbols[j].st_name]);
        }
      }
    }
  }
}


void Print_Symbols(){
  if(total_files == 0){
    printf("You need to examine files first!\n");
  } else if(total_files == 1){
    printf("File: %s\n", file_name1);
    Print_symbols_table(map_start1);
  } else {
    printf("File: %s\n", file_name1);
    Print_symbols_table(map_start1);
    printf("\n");
    printf("File: %s\n", file_name2);
    Print_symbols_table(map_start2);
  }
}

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

Elf32_Shdr* get_section(Elf32_Shdr* sections, char* shstrtab, int sections_size, char* name){
  for(int i = 1; i < sections_size; i++){
    if(strcmp(name, &shstrtab[sections[i].sh_name]) == 0){
      return sections + i;
    }
  }
  return NULL;
}

int prepare_fields(void* map_start, Elf32_Ehdr** header, Elf32_Shdr** shdr, Elf32_Sym** symbols, char** shstrtab, char** strtab, int* symbol_size){
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
  return counter == 1;
}

void Check_Files_for_Merge(){
  //there are two files
  if(total_files != 2){
    printf("You need to examine two files\n");
    return;
  }
  Elf32_Ehdr* header1, *header2;
  Elf32_Shdr* shdr1, *shdr2;
  Elf32_Sym* symbols1, *symbols2;
  char* shstrtab1, *shstrtab2;
  char* strtab1, *strtab2;
  int symbol_size1, symbol_size2;
  
  //there is one symbol table in each
  if(prepare_fields(map_start1, &header1, &shdr1, &symbols1, &shstrtab1, &strtab1, &symbol_size1) == 0){
    printf("Error: Format not supported!\n");
    return;
  }
  if(prepare_fields(map_start2, &header2, &shdr2, &symbols2, &shstrtab2, &strtab2, &symbol_size2) == 0){
    printf("Error: Format not supported!\n");
    return;
  }

  //there are no two undefined and no two defined
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
  printf("---------------\n");
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

void Merge_ELF_Files(){
  //there are two files
  if(total_files != 2){
    printf("You need to examine two files\n");
    return;
  }
  Elf32_Ehdr* header1, *header2;
  Elf32_Shdr* shdr1, *shdr2;
  Elf32_Sym* symbols1, *symbols2;
  char* shstrtab1, *shstrtab2;
  char* strtab1, *strtab2;
  int symbol_size1, symbol_size2;
  
  //there is one symbol table in each
  if(prepare_fields(map_start1, &header1, &shdr1, &symbols1, &shstrtab1, &strtab1, &symbol_size1) == 0){
    printf("Error: Format not supported!\n");
    return;
  }
  if(prepare_fields(map_start2, &header2, &shdr2, &symbols2, &shstrtab2, &strtab2, &symbol_size2) == 0){
    printf("Error: Format not supported!\n");
    return;
  }

  FILE* file = fopen("out.ro", "wb");
  fwrite((char*) header1, 1, header1->e_ehsize, file); //"e_shoff"
  Elf32_Shdr copy_shdr[header1->e_shnum];
  memcpy((char*) copy_shdr, (char*)shdr1, header1->e_shnum * header1->e_shentsize);
  for(int i = 1; i < header1->e_shnum; i++){
    copy_shdr[i].sh_offset = ftell(file);
    if(strcmp(&shstrtab1[shdr1[i].sh_name], ".text") == 0 || strcmp(&shstrtab1[shdr1[i].sh_name], ".data") == 0 || strcmp(&shstrtab1[shdr1[i].sh_name], ".rodata") == 0){
      fwrite((char*)(map_start1 + shdr1[i].sh_offset), 1, shdr1[i].sh_size, file);
      Elf32_Shdr* section = get_section(shdr2, shstrtab2, header2->e_shnum, &shstrtab1[shdr1[i].sh_name]);
      if(section != NULL){
        fwrite((char*)(map_start2 + section->sh_offset), 1, section->sh_size, file);
        copy_shdr[i].sh_size = shdr1[i].sh_size + section->sh_size;
      }
    } else if(strcmp(&shstrtab1[shdr1[i].sh_name], ".symtab") == 0 ){
        Elf32_Sym copy_sym[symbol_size1];
        memcpy((char*) copy_sym, (char*)symbols1, shdr1[i].sh_size);
        for(int j = 1; j < symbol_size1; j++){
          if(symbols1[j].st_shndx == SHN_UNDEF){
            Elf32_Sym* symbol = get_symbol(symbols2, strtab2, symbol_size2, &strtab1[symbols1[j].st_name]);
            copy_sym[j].st_value = symbol->st_value;
            Elf32_Shdr* section = get_section(shdr1, shstrtab1, header1->e_shnum, &shstrtab2[shdr2[symbol->st_shndx].sh_name]);
            copy_sym[j].st_shndx = section - shdr1;
          }
        }
        fwrite((char*) copy_sym, 1, shdr1[i].sh_size, file);
    } else {
      fwrite((char*)(map_start1 + shdr1[i].sh_offset), 1, shdr1[i].sh_size, file);
    }
  }
  int offset = ftell(file);
  fwrite((char*) copy_shdr, 1, header1->e_shnum * header1->e_shentsize, file);
  fseek(file, 32, SEEK_SET); //32 is the offset for e_shoff
  fwrite((char*) (&offset), 1, sizeof(int), file);
  fclose(file);
}

void Quit(){
  if(debug)
  printf("quitting...\n");
  if(total_files >0){
    munmap(map_start1,size1);
    if(total_files >1)
      munmap(map_start2,size2);
  }
  exit(0);
}


int main(int argc, char **argv){
  
  struct fun_desc menu[] = {
    {"Toggle Debug Mode", ToggleDebug_Mode},
    {"Examine ELF File", Examine_ELF_File},
    {"Print Section Names", Print_Section_Names},
    {"Print Symbols", Print_Symbols},
    {"Check Files for Merge", Check_Files_for_Merge},
    {"Merge ELF Files", Merge_ELF_Files},
    {"Quit", Quit},
    {NULL, NULL}
  };

  int upperBound = sizeof(menu) / sizeof(struct fun_desc) - 2 + 48;
  
  while(1){
    printf("Choose action:\n");
    for(int i=0; i<7; i++){
      printf("(%d) %s\n",i,menu[i].name);
    }
    
    char input[50];
    printf("option: ");
    fgets(input,sizeof(input),stdin);

    if (feof(stdin)) { // if ctrl+d entered, we terminate without printing anything.
      break;
    }
    else if(input[0] >= '0' && input[0] <= upperBound){
      printf("Within bounds\n");
    }else{
      printf("Not within bounds\n");
      exit(0);
    }
    
    menu[input[0]-'0'].fun();
    printf("DONE.\n");
  }
  return 0;
}
