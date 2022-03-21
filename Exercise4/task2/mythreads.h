#ifndef __MYTHREADS_H__
#define __MYTHREADS_H__ 

#include <sys/time.h>
#include <signal.h>
#include "mycontext.h"
#define CLOCK_PERIOD 1000

typedef struct thread_s{
	int out_of_order;
	int out_reason;
	ucontext_t cont;
	int thr_name;
	struct thread_s *next;
}thr_t;

typedef struct sems{
	int semid;
	int val;
	int locked;
	int last_up;
}sem_t;

thr_t *header;
ucontext_t *curr_co, *next_co;
int curr_name;
sigset_t set;

int mythreads_init(thr_t *thr_main);
int mythreads_create(thr_t *thr,void func(), void *arg);
int mythreads_yield();
int mythreads_join(thr_t *thr);
int mythreads_destroy(thr_t *thr);
int mythreads_sem_init(sem_t *sema, int value);
int mythreads_sem_down(sem_t *sema);
int mythreads_sem_up(sem_t *sema);
int mythreads_sem_destroy(sem_t *sema);

#endif
