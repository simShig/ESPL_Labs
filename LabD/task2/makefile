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

all: task2

# (target) : (dependencies)
task2 :  task2.o main.o
#	@echo "making exe"
	$(complC) task2 task2.o main.o

task2.o : task2.s 
#	@echo "making task0.o"
	$(complS) task2.o task2.s

main.o : main.s
#	@echo "making main.o"
	$(complS) main.o main.s
clean:
	rm -f *.o task2 main
