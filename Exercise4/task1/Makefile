all: task1

task1: task1.o mycontext.o
	gcc -Wall -g task1.o mycontext.o -o task1 -lpthread

task1.o: task1.c mycontext.h
	gcc -Wall -g -c task1.c

mycontext.o: mycontext.c mycontext.h
	gcc -Wall -g -c mycontext.c
