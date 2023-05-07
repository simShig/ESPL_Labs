all: task1b

task1b: task1b.o util.o
	gcc -g -Wall -m32 -o task1b task1b.o util.o

task1b.o:task1b.s 
	nasm -g -f elf32 task1b.s -o task1b.o

util.o:util.c 
	gcc -m32 -g -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

.PHONY: clean 

clean: 
	rm -f *.o task1b util
