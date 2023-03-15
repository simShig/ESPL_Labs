all: clean t1a

t1a: addresses.o
	gcc -g -Wall -m32 -o t1a addresses.o

addresses.o: addresses.c
	gcc -g -Wall -m32 -c -o addresses.o addresses.c

.PHONY: clean 
clean:
	rm -f *.o t1a

# questions:

# Can you tell the location (stack, code, etc.) of each memory address?

# What can you say about the numerical values?Do they obey a particular order?

# Check long data size on your machine using sizeof operator.
# Is long integer data type enough for dist (address difference) variables ?