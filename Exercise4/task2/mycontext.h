#ifndef __MYCONTEXT_H__
#define __MYCONTEXT_H__ 


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ucontext.h>
#define STACK_SIZE 16384
#define SIZE 6

int mycoroutines_init(ucontext_t *main_con);
int mycoroutines_create(ucontext_t *co, void func(),void* arg, ucontext_t *next);
int mycoroutines_switchto(ucontext_t *old_co, ucontext_t *new_co);
int mycoroutines_destroy(ucontext_t *co);

#endif
