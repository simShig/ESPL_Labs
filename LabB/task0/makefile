all: hexaPrint 

hexaPrint:hexaPrint.o
	gcc -g -Wall -m32 -o hexaPrint hexaPrint.o

hexaPrint.o:hexaPrint.c 
	gcc -g -Wall -m32 -c -o hexaPrint.o hexaPrint.c

.PHONY: clean 

clean: 
	rm -f *.o hexaPrint 
