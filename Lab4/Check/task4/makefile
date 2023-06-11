all : digit_count hexeditplus
digit_count: digit_count.o
	gcc -m32 -g -Wall -o digit_count digit_count.o
digit_count.o: digit_count.c
	gcc -m32 -g -Wall -fno-pie -fno-stack-protector -m32 -c -o digit_count.o digit_count.c
hexeditplus: hexeditplus.o
	gcc -m32 -g -Wall -o hexeditplus hexeditplus.o
hexeditplus.o: hexeditplus.c
	gcc -m32 -g -Wall -c -o hexeditplus.o hexeditplus.c

.PHONY: clean

clean:
	rm digit_count digit_count.o
	rm hexeditplus hexeditplus.o
