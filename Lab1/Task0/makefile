all: clean cw

cw: count_words.o
	gcc -g -Wall -m32 -o cw count_words.o

count_words.o: count-words.c 
	gcc -c -g -Wall -m32 -o count_words.o count-words.c

.PHONY: clean 
clean:
	rm -f *.o cw 