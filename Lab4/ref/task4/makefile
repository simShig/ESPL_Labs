all : digit_count
digit_count: digit_count.o
	gcc -m32 -g -Wall -o digit_count digit_count.o
digit_count.o: digit_count.c
	gcc -m32 -g -Wall -fno-pie -m32 -c -o digit_count.o digit_count.c

.PHONY: clean

clean:
	rm digit_count digit_count.o
