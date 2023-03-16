all: clean t2a

t2a: base.o
	gcc -g -Wall -m32 -o t2a base.o

base.o: base.c
	gcc -g -Wall -m32 -c -o base.o base.c

.PHONY: clean 
clean:
	rm -f *.o t2a
