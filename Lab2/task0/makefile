
all: myshell

myshell: myshell.o LineParser.o looper.o
	gcc -m32 -g -Wall -o myshell myshell.o LineParser.o

# LineParser: LineParser.o
# 	gcc -m32 -g -Wall -o LineParser LineParser.o

myshell.o: myshell.c
	gcc -g -Wall -m32 -c -o myshell.o myshell.c

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c

looper.o: looper.c
	gcc -g -Wall -m32 -c -o looper.o looper.c

.PHONY: clean 


clean: 
	rm -f *.o myshell LineParser looper
