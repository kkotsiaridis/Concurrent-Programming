all: task3

task3: task3.o mysem.o
	gcc -Wall -g task3.o mysem.o -o task3 -lpthread

task3.o: task3.c mysem.h
	gcc -Wall -g -c task3.c

mysem.o: mysem.c mysem.h
	gcc -Wall -g -c mysem.c
