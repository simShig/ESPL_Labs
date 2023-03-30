all: virusDetector 

virusDetector:virusDetector.o
	gcc -g -Wall -m32 -o virusDetector virusDetector.o

virusDetector.o:virusDetector.c 
	gcc -g -Wall -m32 -c -o virusDetector.o virusDetector.c

.PHONY: clean 

clean: 
	rm -f *.o virusDetector 
