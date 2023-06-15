#define _FILE_OFFSET_BITS 64    //refferance : https://stackoverflow.com/questions/13893580/calling-stat-from-sys-stat-h-faills-with-value-too-large-for-defined-data-typ
#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

extern int startup(int argc, char **argv, void (*start)());     //extern - defined in given assembly file - startup.s

//~~~static Vars~~~

Elf32_Ehdr *header; 
int usedfd =-1;
char* myFile=NULL;
struct stat fileStat; /* to get fileStat.st_size */
void* map_start; /* will point to the start of the memory mapped file */
//~~~PHDR values extraction:~~~~
char* Flag2String(int flag){         //for flag printing
        if(flag==0x000) return "";
        if(flag==0x001) return "E";
        if(flag==0x002) return "W";
        if(flag==0x003) return "WE";
        if(flag==0x004) return "R";
        if(flag==0x005) return "RE";
        if(flag==0x006) return "RW";
        if(flag==0x007) return "RWE";
        return "Unknown";
    }


int Flag4Use(int flag){        //for flag using
        if(flag==0x000) return 0;
        if(flag==0x001) return PROT_EXEC;
        if(flag==0x002) return PROT_WRITE;
        if(flag==0x003) return PROT_WRITE | PROT_EXEC;
        if(flag==0x004) return PROT_READ;
        if(flag==0x005) return PROT_READ | PROT_EXEC;
        if(flag==0x006) return PROT_READ | PROT_WRITE;
        if(flag==0x007) return PROT_READ | PROT_WRITE | PROT_EXEC;
        return -1;
    }

char* Type2String(int type){        //for type printing - refferance: https://github.com/finixbit/elf-parser/blob/master/elf_parser.cpp
    switch (type){
        case PT_NULL: return "NULL";
        case PT_LOAD: return "LOAD";
        case PT_DYNAMIC: return "DYNAMIC";
        case PT_INTERP: return "INTERP";
        case PT_NOTE: return "NOTE";
        case PT_SHLIB: return "SHLIB";
        case PT_PHDR: return "PHDR";
        case PT_TLS: return "TLS";
        case PT_NUM: return "NUM";
        case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";
        case PT_GNU_STACK: return "GNU_STACK";
        case PT_GNU_RELRO: return "GNU_RELRO";
        case PT_SUNWBSS: return "SUNWBSS";
        case PT_SUNWSTACK: return "SUNWSTACK";
        case PT_HIOS: return "HIOS";
        case PT_LOPROC: return "LOPROC";
        case PT_HIPROC: return "HIPROC"; 
        default:return "Unknown";
    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


void load_phdr(Elf32_Phdr *phdr, int fd){
    if(phdr -> p_type == PT_LOAD){
        int permFlags = Flag4Use(phdr -> p_flags);
            //~~from reading material:~~
        void *vadd = (void *)(phdr -> p_vaddr&0xfffff000);
        int offset = phdr -> p_offset&0xfffff000;
        int padding = phdr -> p_vaddr & 0xfff;
        void* temp;
        if ((temp = mmap(vadd, phdr -> p_memsz+padding, permFlags, MAP_FIXED | MAP_PRIVATE, fd, offset)) == MAP_FAILED ) {
            perror("mmap failed!");
            exit(-1);
        }
        printf("%s\t\t%#08x\t%#08x\t%#08x\t%#07x\t\t%#07x\t\t%s\t%#05x\n",
            Type2String(phdr->p_type),phdr->p_offset,phdr->p_vaddr,phdr->p_paddr,phdr->p_filesz,phdr->p_memsz,Flag2String(phdr->p_flags),phdr->p_align);  
    }
    else{       //case type not Load
         printf("%s\t%#08x\t%#08x\t%#08x\t%#07x\t\t%#07x\t\t%s\t%#05x\n",
            Type2String(phdr->p_type),phdr->p_offset,phdr->p_vaddr,phdr->p_paddr,phdr->p_filesz,phdr->p_memsz,Flag2String(phdr->p_flags),phdr->p_align);  
    }
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg){
    int num = header->e_phnum;
    int offset = header->e_phoff;
    int size = header->e_phentsize;
    printf("Type\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\t\tMemSiz\t\tFlg\tAlign\n");
    for (int i = 0; i < num; i++){       
        Elf32_Phdr* entry = map_start + offset + (i * size);   
        func(entry,arg);
    }
    return 0;
}

int main(int argc, char ** argv){
    int fd;
    if(argc < 2){
        printf("not enough arguments - filename needed\n");
        exit(1);
    }
    if((fd = open(argv[1], O_RDWR)) < 0) {
      perror("error in open");
      exit(-1);
   }
    if(fstat(fd, &fileStat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }
    if ((map_start = mmap(0, fileStat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-1);
   }
    if(usedfd!=-1)
        close(usedfd);
	strcpy((char*)&myFile,(char*)argv[1]);
    header = (Elf32_Ehdr *) map_start;
    usedfd = fd;
    foreach_phdr(map_start, load_phdr, usedfd);
    startup(argc-1, argv+1, (void *)(header->e_entry));
}
