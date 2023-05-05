all: task2

task2: start.o util.o main.o
### gcc -g -Wall -m32 -o task2a start.o util.o main.o
	ld -m elf_i386 start.o main.o util.o -o task2

start.o:start.s 
	nasm -f elf32 start.s -o start.o

util.o:util.c 
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

main.o:main.c 
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o


.PHONY: clean 

clean: 
	rm -f *.o start 
	rm -f *.o util
	rm -f *.o main 
	rm -f *.o task2
