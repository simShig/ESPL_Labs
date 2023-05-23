# Readme:
# gcc : gnu c/c++ compiler
# gcc flags:
# 	-m32 : use 32bit libraries
# 	-g : add debug info - no flag no debug
# 	-Wall : warning level - all
# 	-c : no linking (just compile)
# 	-o : optimize
# nasm : assembly compiler

# macros:
compC = gcc -no-pie -m32 -g -Wall -c -o
complC = gcc -no-pie -m32 -g -Wall -o
complS = nasm -g -f elf -w+all -o

.PHONY : clean

all: task1

# (target) : (dependencies)
task1 :  task1.o
#	@echo "making exe"
	$(complC) task1 task1.o

task1.o : task1.s
#	@echo "making task0.o"
	$(complS) task1.o task1.s

clean:
	rm -f *.o task1
