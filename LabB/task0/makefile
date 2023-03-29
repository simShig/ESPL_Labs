all: bubblesort 

bubblesort:bubblesort.o
	gcc -g -Wall -m32 -o bubblesort bubblesort.o

bubblesort.o:bubblesort.c 
	gcc -g -Wall -m32 -c -o bubblesort.o bubblesort.c

.PHONY: clean 

clean: 
	rm -f *.o bubblesort 
