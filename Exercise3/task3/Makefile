all: task3

task3: task3.o mtx_cond.o
	gcc -Wall -g task3.o mtx_cond.o -o task3 -lpthread

task3.o: task3.c mtx_cond.h
	gcc -Wall -g -c task3.c

mtx_cond.o: mtx_cond.c mtx_cond.h
	gcc -Wall -g -c mtx_cond.c
