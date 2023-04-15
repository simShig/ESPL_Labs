all: task0b

task0b: looper.o
	gcc -g -m32 -Wall -o task0b looper.o


looper.o: looper.c
	gcc -m32 -Wall -c looper.c -o looper.o
	
.PHONY: clean
clean:
	rm -f *.o looper