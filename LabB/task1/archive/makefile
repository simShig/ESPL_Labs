all: gpts 

gpts:gpts.o
	gcc -g -Wall -m32 -o gpts gpts.o

gpts.o:gpts.c 
	gcc -g -Wall -m32 -c -o gpts.o gpts.c

.PHONY: clean 

clean: 
	rm -f *.o gpts 
