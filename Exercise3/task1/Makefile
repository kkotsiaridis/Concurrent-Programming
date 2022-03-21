all: task1

task1: task1.o mtx_cond.o
	gcc -Wall -g task1.o mtx_cond.o -o task1 -lpthread

task1.o: task1.c mtx_cond.h
	gcc -Wall -g -c task1.c

mtx_cond.o: mtx_cond.c mtx_cond.h
	gcc -Wall -g -c mtx_cond.c
