all: task2

task2: task2.o mysem.o
	gcc -Wall -g task2.o mysem.o -o task2 -lpthread

task2.o: task2.c mysem.h
	gcc -Wall -g -c task2.c

mysem.o: mysem.c mysem.h
	gcc -Wall -g -c mysem.c
