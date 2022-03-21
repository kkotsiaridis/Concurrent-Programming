/*karampinis ioannis 2529
 * kotsiaridis konstantinos 2547*/

#include "mythreads.h"
#include "mycontext.h"
#include <unistd.h>


struct informations{
	int isprime;
	int isready;
	int number;
    int onoma;
};

int waitingWorkers, times, flag, termination;
sem_t **workerSems, *mtx, *waitingMain;
int *terminated, terminatingSequence;


int primetest(int number){
	int i;
	
	if(number==0){
		return(0);
	}
	if(number==1){
		return(1);
	}
	for(i=2; i<=number/2; i++){
		if((number%i)==0){
			return(0);
		}
	}
	return(1);
}


void primesearch(void *arg){
	int num=-2,isfirst=1;
	struct informations *temp;
    struct timeval t0, t1, dt;
	
	
	temp = (struct informations*)arg;
	
	printf("-----------------------Start of worker: %d---------------------------\n", temp->onoma);
     
	while(1){
		if(isfirst!=1){
			mythreads_sem_up(mtx);

			mythreads_sem_down(workerSems[temp->onoma]);
			//elegxos termatismou thread
			if(temp->number==-1){
				mythreads_sem_down(mtx);
				terminated[temp->onoma]=1;
				printf("_________________Termination of thread: %d__________________\n",temp->onoma);
				waitingWorkers--;
				if(waitingWorkers>0 && terminatingSequence == 1){
					mythreads_sem_up(waitingMain);
				}
				mythreads_sem_up(mtx);
				return;
			}
			mythreads_sem_down(mtx);
			//worker in work
			waitingWorkers--;
			if(waitingWorkers>0){
				mythreads_sem_up(waitingMain);
			}
			mythreads_sem_up(mtx);
		}
		else{
			/*prvth fora pou trexei o worker*/
			isfirst=0;
		}
       
		gettimeofday(&t0, NULL);
		num=temp->number;
		temp->isprime = primetest(num);
		
		mythreads_sem_down(mtx);
		times++;
		printf("->%d\n",times);
		if(temp->isprime == 0){
			printf("(%d):   The number: %d is NOT prime!\n",temp->onoma, num);
		}
		else{
			printf("(%d):   The number: %d is prime!\n", temp->onoma, num);
		}
        gettimeofday(&t1, NULL);
        timersub(&t1, &t0, &dt);
        printf("Thread -%d-  took %d.%012d sec(number->(%d))\n\n", temp->onoma, (int)dt.tv_sec, (int)dt.tv_usec, num);
		//worker diathesimos pali
		waitingWorkers++;
        temp->isready = 1;
		if(waitingWorkers==1){
			/*dinei shma oti enas worker einai pleon dia8esimos*/
			mythreads_sem_up(waitingMain);
		}
	}
}

void handler(void *arg){
	int check,k;
	thr_t *search,*temp;
	
	temp=(thr_t *)arg;
	
	while(1){
		search = header;
		//check if the only thread in list is main-->stuck in infinite loop
		if(search->thr_name==search->next->thr_name){
			printf("Something wrong happened\n");
				break;
		}
		//searching for the current thread in list
		while(search->thr_name!=curr_name){
			search=search->next;
		}
			
		printf("prev_name_handler: %d\n", search->thr_name);
		//searching for the next available thread that is not the handler
		do{
			search = search->next;
			while(search->out_of_order!=0){
				search = search->next;
			}
		}while(search->thr_name == temp->thr_name);
		//updating curr_name to be the name of next running thread
		curr_name = search->thr_name;
		printf("curr_name_handler: %d\n", curr_name);
		
		//switching to next thread and storing handler's current 
		//state at next_co for SIGALRM_handler to use in switchto
		check = mycoroutines_switchto(next_co,&search->cont);
		if(check==-1){
			printf("Error in handler!\n");
			return;
		}
	
		search=header->next->next;
		for(k=0; k<flag; k++){
			if(terminated[k]==1){
				search->out_of_order=-1;
			}
			search=search->next;
		}
		if(termination == 1){
			break;
		}
	}
	
	printf("_________________Termination of HANDLER________________\n");
	return;
}

int main(int argc, char* argv[]){
	int i, k, temp, num = atoi(argv[1]), check;
	struct informations *workers[num];
	thr_t worker_thr[num], thr_main, thr_handler;
    struct timeval k0, k1, dk;
     
    gettimeofday(&k0, NULL);
	
	flag = 0;
	termination = 0;
	waitingWorkers = 0;
	times = 0;
	terminatingSequence=0;

	terminated=(int *)malloc(num * sizeof(int));
	if(terminated==NULL){
		printf("Memory allocation problem in terminated\n");
		return(1);
	}
	for(i=0;i<num;i++){
		terminated[i] = 0;
	}
	
	for(i=0; i<num; i++){
		workers[i]=(struct informations*)malloc(sizeof(struct informations));
		if(workers[i] == NULL){
			printf("Malloc %d in main for workers!\n", i);
			for(k=0; k<i; k++){
				free(workers[k]);
			}
			return(1);
		}
	}
	workerSems=(sem_t **)malloc(num* sizeof(sem_t*));
	if(workerSems==NULL){
		printf("Memory allocation problem in workerSems!\n");
		return(1);
	}
	mtx = (sem_t *)malloc(sizeof(sem_t));
	if(mtx==NULL){
		printf("Memory allocation problem in mtx!\n");
		return(1);
	}
	waitingMain = (sem_t *)malloc(sizeof(sem_t));
	if(waitingMain==NULL){
		printf("Memory allocation problem in waitingMain!\n");
		return(1);
	}
	for(i=0;i<num;i++){
		workerSems[i] = (sem_t *)malloc(sizeof(sem_t));
		if(workerSems[i]==NULL){
			printf("Memory allocation problem in workerSems[%d]!\n",i);
			return(1);
		}
		mythreads_sem_init(workerSems[i], 0);
	}
	mythreads_sem_init(mtx, 1);
	mythreads_sem_init(waitingMain, 0);
	

	check = mythreads_init(&thr_main);
	if(check == -1){
		return(-1);
	}
	curr_co = &thr_main.cont;
	curr_name = 0;

	check = mythreads_create(&thr_handler,handler,&thr_handler);
	if(check == -1){
		return(-1);
	}
	
 	next_co = &thr_handler.cont;
	
	check = sigprocmask(SIG_UNBLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return(-1);
	}
	/*dinei se ka8e worker enan ari8mo kai tous 8etei se leitourgeia*/
	for(i=0; i<num; i++){
		check = scanf(" %d", &temp);
		if(check == EOF){
			break;
		}
		flag++;/*se periptwsh pou ai8moi eisodou<threads pou dhmiourgoume*/
		
		workers[i]->number = temp;
		workers[i]->isprime = -1;
		workers[i]->isready = 0;
        workers[i]->onoma = i;
		
		check = sigprocmask(SIG_BLOCK, &set, NULL);
		if(check==-1){
			printf("Error in sigprocmask!\n");
			return(-1);
		}
		
		check = mythreads_create(&worker_thr[i], primesearch, workers[i]);
		if(check == -1){
			for(k=0; k<num; k++){
				free(workers[k]);
			}
			return(1);
		}
		printf("Creation of worker %d\n",i);
		check = sigprocmask(SIG_UNBLOCK, &set, NULL);
		if(check==-1){
			printf("Error in sigprocmask!\n");
			return(-1);
		}
	}
	
	while(1){
		check = scanf(" %d", &temp);
		if(check == EOF){
			break;
		}

		check = mythreads_sem_down(waitingMain);
		if(check == -1){
			return(1);
		}
		
		for(i=0; i<flag; i++){
			if(workers[i]->isready==1){
				workers[i]->number = temp;
				workers[i]->isready = 0;
				workers[i]->isprime = -1;
				check = mythreads_sem_up(workerSems[i]);
				if(check == -1){
					return(1);
				}
				break;
			}
		}
	}
	
	printf("+++++++++++++++++NUMBERS HAVE FINISHED++++++++++++++++\n");
	terminatingSequence = 1;
	//apostolh shmatwn termatismou
	for(k=0;k<flag;k++){

		check = mythreads_sem_down(waitingMain);
		if(check == -1){
			return(1);
		}
		
		for(i=0; i<flag; i++){
			if(workers[i]->isready==1){
				workers[i]->number = -1;
				workers[i]->isready = 0;
				workers[i]->isprime = -1;
				check = mythreads_sem_up(workerSems[i]);
				if(check == -1){
					return(1);
				}
				break;
			}
		}
	}
	check = sigprocmask(SIG_BLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return(-1);
	}

	/*perimenei mexri na teleiwsoun oloi oi workers*/
	printf("I'm waiting for workers to finish.\n");
	//mythreads_yield();
	for(i=0; i<flag; i++){
		mythreads_join(&worker_thr[i]);
	}
	check = sigprocmask(SIG_BLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return(-1);
	}
	
	for(i=0; i<flag; i++){
		printf("enter destroy[%d]\n",i);
		mythreads_destroy(&worker_thr[i]);
	}
	printf("All threads are terminated\n");
	termination = 1;
	mythreads_join(&thr_handler);
	
	check = sigprocmask(SIG_BLOCK, &set, NULL);
	if(check==-1){
		printf("Error in sigprocmask!\n");
		return(-1);
	}
	for(i=0;i<num;i++){        
        free(workers[i]);
    }

	gettimeofday(&k1, NULL);
    timersub(&k1, &k0, &dk);
        
    printf("Main took %d.%012d secs\n", (int)dk.tv_sec, (int)dk.tv_usec);
	printf("->%d\n",times);
	
	mythreads_sem_destroy(mtx);
	mythreads_sem_destroy(waitingMain);
	for(i=0;i<num;i++){
		mythreads_sem_destroy(workerSems[i]);
	}
	free(workerSems);
	
	return(0);
}
