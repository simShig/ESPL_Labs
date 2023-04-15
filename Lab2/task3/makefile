all: task3 looper

task3: myshell.o LineParser.o
	gcc -g -m32 -Wall -o task3 myshell.o LineParser.o

myshell.o: myshell.c
	gcc -m32 -Wall -c myshell.c -o myshell.o

LineParser.o: LineParser.c
	gcc -m32 -Wall -c LineParser.c -o LineParser.o
	
looper: looper.c
	gcc -m32 -Wall looper.c -o looper	

.PHONY: clean
clean:
	rm -f *.o task3
	rm -f *.o looper