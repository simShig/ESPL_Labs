#format is target-name: target dependencies 

 

#{-tab-}actions 

 # All Targets 

all: hello 

 # Tool invocations 

# Executable "hello" depends on the files hello.o and run.o. 

hello: hello.o Run.o gcc -g -m32 -Wall -o hello hello.o Run.o 

 # Depends on the source and header files 

hello.o: HelloWorld.c HelloWorld.h gcc -m32 -g -Wall -c -o hello.o HelloWorld.c 

Run.o: Run.c gcc -m32 -g -Wall -c -o Run.o Run.c 

 #tell make that "clean" is not a file name! 

.PHONY: clean 

 #Clean the build directory 

clean: rm -f *.o hello

 