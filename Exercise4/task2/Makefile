all: task2

task2: task2.o mythreads.o mycontext.o 
	gcc -Wall -g task2.o mythreads.o mycontext.o -o task2 -lpthread

task2.o: task2.c mythreads.h mycontext.h
	gcc -Wall -g -c task2.c

mythreads.o: mythreads.c mythreads.h mycontext.h
	gcc -Wall -g -c mythreads.c

mycontext.o: mycontext.c mycontext.h
	gcc -Wall -g -c mycontext.c
