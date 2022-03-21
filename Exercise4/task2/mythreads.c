/*kotsiaridis konstantinos 2547
 *karampinis ioannis 2529*/

#include "mythreads.h"

struct sigaction act={{0}};
struct itimerval t={{0}};
int semid_counter = 1000;
int thr_name_counter = 0;



static void SIGALRM_handler(int sig){
	thr_t *search;
	int check;
	
	check = sigprocmask(SIG_BLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return;
	}
	
	search=header;
	while(search->thr_name!=curr_name){
		search=search->next;
	}
	printf("I got SIGALRM\n");
	
	mycoroutines_switchto(&search->cont,next_co);
	
	check = sigprocmask(SIG_UNBLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return;
	}
}

int mythreads_init(thr_t *thr_main){
	int check;
	
	check = mycoroutines_init(&thr_main->cont);
	if(check==-1){
		return(-1);
	}

	header=thr_main;
	
	thr_main->next=header;
	thr_main->out_of_order = 0;
	thr_main->thr_name = thr_name_counter;
	thr_name_counter++;
	
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	
	act.sa_handler= SIGALRM_handler;
    sigaction(SIGALRM,&act,NULL);
	
	check = sigprocmask(SIG_BLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return(-1);
	}
	
	t.it_value.tv_sec= 0;
    t.it_value.tv_usec= CLOCK_PERIOD;
    t.it_interval.tv_sec= 0;
    t.it_interval.tv_usec= CLOCK_PERIOD;
    
    setitimer(ITIMER_REAL,&t,NULL);
	
	return(0);
}

int mythreads_create(thr_t *thr,void func(), void *arg){
	int check;
	thr_t *search;
	if(thr_name_counter==1){ //handler
		check = mycoroutines_create(&thr->cont, func, arg,curr_co);
		if(check==-1){
			return(-1);
		}
	}
	else{ //workers
		search=header->next;
		check = mycoroutines_create(&thr->cont, func, arg,&search->cont);
		if(check==-1){
			return(-1);
		}
	}
	
	thr->next = header;
	thr->out_of_order = 0;
	thr->out_reason = 0;
	thr->thr_name = thr_name_counter;
	thr_name_counter++;
	
	search = header;
	while(search->next != header){
		search = search->next;
	}
	search->next = thr; //place at the end of the list
	
	return(0);
}

int mythreads_yield(){
	thr_t *search;
	int check;
	
	check = sigprocmask(SIG_BLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return(-1);
	}
	//restart the timer for SIGALRM
	t.it_value.tv_sec= 0;
    t.it_value.tv_usec= CLOCK_PERIOD;
    t.it_interval.tv_sec= 0;
    t.it_interval.tv_usec= CLOCK_PERIOD;
    
    setitimer(ITIMER_REAL,&t,NULL);
	
	search=header;
	while(search->thr_name!=curr_name){
		search=search->next;
	}
	
	printf("I got YIELD\n");
	mycoroutines_switchto(&search->cont,next_co);
	
	check = sigprocmask(SIG_UNBLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return(-1);
	}
	return(0);
}

int mythreads_join(thr_t *thr){
	thr_t *search;
	int check;
	//check if thread has been removed from the list
	search=header;
	while(search->thr_name != thr->thr_name){
		if(search->next->thr_name==header->thr_name){
			printf("Thread doesn't exist!\n");
			return(0);
		}
		search=search->next;
	}
	//wait for the termination of the worker
	while(thr->out_of_order!=-1){
		mythreads_yield();
		check = sigprocmask(SIG_BLOCK, &set, NULL);
		if(check==-1){
			printf("Error in sigprocmask!\n");
			return(-1);
		}
	}
	
	return(0);
}

int mythreads_destroy(thr_t *thr){
	thr_t *search, *temp;
	//remove the node from the list
	search = header;
	while(search->next->thr_name != thr->thr_name){
		search = search->next;
	}
	temp = search->next;
	search->next = temp->next;
	
	mycoroutines_destroy(&thr->cont);
	
	return(0);
}

int mythreads_sem_init(sem_t *sema, int value){

	sema->locked=0;
	sema->val = value;
	sema->semid = semid_counter;
	sema->last_up = 0;
	semid_counter++;
	return(0);
}

int mythreads_sem_down(sem_t *sema){
	int check=0;
	thr_t *search;

	while(sema->locked !=0){}
	sema->locked=1;
	
	if(sema->val > 0){
		sema->val--;
		sema->locked=0;
		return(0);
	}
	
	search = header;
	while(search->thr_name != curr_name){
		search = search->next;
	}
	search->out_of_order = 1;
	search->out_reason = sema->semid;
	sema->locked = 0;
	//printf("i go to sleep:%d\n",sema->semid);
	check = mythreads_yield();
	if(check==-1){
		return(-1);
	}
	return(0);
}

int mythreads_sem_up(sem_t *sema){
	thr_t  *search;
	
	//lock sem's internal mtx and before return unlock it
	while(sema->locked !=0){}
	sema->locked=1;
	
	//increment sem's val if val>=1 and return
	if(sema->val > 0){
		sema->val++;
		sema->locked=0;
		return(0);
	}
	//find sem's last_up in list
	search = header;
	while(search->thr_name != sema->last_up){
		if(search->next == header){
			break;
		}
		search=search->next;
	}
	search=search->next;
	
	//from starting point=last_up->next :start searching for someone to wake up 
	//if search successfull break
	while(search->out_reason != sema->semid){
		if(search->next->thr_name== sema->last_up){           //if you complete a full circle in list
			if(search->next->out_reason != sema->semid){ //if last_up isn't asleep increase sem's val and return
				sema->val++;
				sema->last_up = header->thr_name;
				sema->locked=0;
				return(0);
			}
			else{                                        //if last_up is asleep break
				search=search->next;
				break;
			}
		}
		else{
			search=search->next;
		}
	}
	
	//wake up the sleeping thread and update last_up
	search->out_reason = 0;
	search->out_of_order = 0;
	sema->last_up = search->thr_name;
	sema->locked=0;
	return(0);
}

int mythreads_sem_destroy(sem_t *sema){
	thr_t *search;
	
	if(sema->val > 0){
		free(sema);
		return(0);
	}
	//check if semaphore is still in use
	search = header;
	while(search->out_reason != sema->semid){
		if(search->next==header){
			free(sema);
			return(0);
		}
		else{
			search=search->next;
		}
	}
	printf("Error: semaphore still in use!\n");
	free(sema);
	return(-1);
}
