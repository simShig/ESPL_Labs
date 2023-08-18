all: task1

task1: start.o util.o encoder.o
### gcc -g -Wall -m32 -o task1a start.o util.o encoder.o
	ld -m elf_i386 start.o encoder.o util.o -o task1

start.o:start.s 
	nasm -f elf32 start.s -o start.o

util.o:util.c 
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

encoder.o:encoder.s 
	nasm -f elf32 encoder.s -o encoder.o


.PHONY: clean 

clean: 
	rm -f *.o start 
	rm -f *.o util
	rm -f *.o encoder 
	rm -f *.o task1
