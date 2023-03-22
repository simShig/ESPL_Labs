all: debug 

debug:debug.o
	gcc -m32 -g -Wall -o debug debug.o

debug.o:debug.c 
	gcc -g -Wall -m32 -c -o debug.o debug.c

.PHONY: clean 

clean: 
	rm -f *.o debug 
