all: myshell looper mypipe

myshell: myshell.o LineParser.o
	gcc -g  -Wall -o myshell myshell.o LineParser.o

myshell.o: myshell.c
	gcc  -Wall -c myshell.c -o myshell.o

LineParser.o: LineParser.c
	gcc  -Wall -c LineParser.c -o LineParser.o

mypipe: mypipe.c
	gcc  -Wall mypipe.c -o mypipe

looper: looper.c
	gcc  -Wall looper.c -o looper
	
.PHONY: clean
clean:
	rm -f *.o myshell
	rm -f *.o looper
	rm -f *.o mypipe