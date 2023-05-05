all: encoder

encoder: encoder.o util.o
	gcc -g -Wall -m32 -o encoder encoder.o util.o

encoder.o:encoder.s 
	nasm -g -f elf32 encoder.s -o encoder.o

util.o:util.c 
	gcc -m32 -g -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

.PHONY: clean 

clean: 
	rm -f *.o encoder util
