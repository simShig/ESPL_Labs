all: loader

loader.o: loader.c startup.o start.o
	gcc -c loader.c -o loader.o
	ld -o loader loader.o startup.o start.o -lc -T linking_script -dynamic-linker /lib/ld-linux.so.2 
	

start: start.o
	 gcc -g -m32 -Wall -o start start.o

 # Depends on the source and header files 

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s  

startup: startup.o
	 gcc -g -m32 -Wall -o startup startup.o

 # Depends on the source and header files 

startup.o: startup.s
	nasm -g -f elf -w+all -o startup.o startup.s  

.PHONY: clean

clean:
	rm -f *.o loader
