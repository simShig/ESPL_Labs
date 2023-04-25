all: mypipeline 

mypipeline:mypipeline.o
	gcc -g -Wall -m32 -o mypipeline mypipeline.o

mypipeline.o:mypipeline.c 
	gcc -g -Wall -m32 -c -o mypipeline.o mypipeline.c

.PHONY: clean 

clean: 
	rm -f *.o mypipeline 
