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

all: task3b

# (target) : (dependencies)
task3b :  task3b.o task2.o task3a.o
#	@echo "making exe"
	$(complC) task3b task3b.o task2.o task3a.o

task3b.o : task3b.s
#	@echo "making task3b.o"
	$(complS) task3b.o task3b.s

task2.o : task2.s
#	@echo "making task2.o"
	$(complS) task2.o task2.s

task3a.o : task3a.s
#	@echo "making task3a.o"
	$(complS) task3a.o task3a.s

clean:
	rm -f *.o task3b task2 task3a
