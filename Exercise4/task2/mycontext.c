#include "mycontext.h"

int mycoroutines_init(ucontext_t *main_con){
	int check;
	
	printf("inside mycoroutines_init\n");

	check = getcontext(main_con);
	if(check==-1){
		printf("Problem in mycoroutines_init!\n");
		return(-1);
	}
	printf("inside mycoroutines_init\n");
	return(0);
}

int mycoroutines_create(ucontext_t *co, void func(),void* arg, ucontext_t *next){
	int check;
	
	check = getcontext(co);
	if(check==-1){
		printf("Problem in mycoroutines_create-getcontext!\n");
		return(-1);
	}
	
	co->uc_link=next;
	co->uc_stack.ss_sp=(char*)malloc(STACK_SIZE);
	if(co->uc_stack.ss_sp==NULL){
		printf("Problem with memory allocation in mycoroutines_create!\n");
		return(-1);
	}
	co->uc_stack.ss_size=STACK_SIZE;
	co->uc_stack.ss_flags=0;
	makecontext(co,func,1,arg);
	
	return(0);
}

int mycoroutines_switchto(ucontext_t *old_co, ucontext_t *new_co){
	int check;
	
	//printf("inside mycoroutines_switchto\n");
	check = swapcontext(old_co, new_co);
	if(check==-1){
		printf("Problem in mycoroutines_switchto!\n");
		return(-1);
	}
	//printf("inside mycoroutines_switchto\n");
	return(0);
}
	
int mycoroutines_destroy(ucontext_t *co){
	
	free(co->uc_stack.ss_sp);
	co->uc_link=NULL;
	co->uc_stack.ss_size=0;
	//free(co);
	
	return(0);
}
