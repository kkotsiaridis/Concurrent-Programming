all: task2

task2: task2.o mtx_cond.o
	gcc -Wall -g task2.o mtx_cond.o -o task2 -lpthread

task2.o: task2.c mtx_cond.h
	gcc -Wall -g -c task2.c

mtx_cond.o: mtx_cond.c mtx_cond.h
	gcc -Wall -g -c mtx_cond.c
