all: task4

task4: task4.o mysem.o
	gcc -Wall -g task4.o mysem.o -o task4 -lpthread

task4.o: task4.c mysem.h
	gcc -Wall -g -c task4.c

mysem.o: mysem.c mysem.h
	gcc -Wall -g -c mysem.c
