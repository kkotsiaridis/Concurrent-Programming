/*karampinis ioannis 2529
 * kotsiaridis konstantinos 2547*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mysem.h"

struct informations{
	int isprime;
	int isready;
	int number;
    int onoma;
};

int terminating, terminatedWorkers;
int waitingMain, terminatingMain, flag;
int workersQueue, mtx, waitingWorkers, times,wasted_ups;

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


void* primesearch(void *arg){
	int num=-1, isfirst=1;
	struct informations *temp;
    struct timeval t0, t1, dt;
	
	temp = (struct informations*)arg;
	
	printf("-----------------------Start of worker: %d---------------------------\n", temp->onoma);
     
	while(1){
		if(isfirst!=1){
			/*elegxos an teleiwse o teleutaios worker*/
			if((terminating==1)&&(waitingWorkers==flag)){
				wasted_ups+=mysem_up(waitingMain,__LINE__);
			}
			wasted_ups+=mysem_up(mtx,__LINE__);
			
			while(1){
				
				mysem_down(workersQueue);
				
                if(terminating==-2){
                    printf("Termination of thread: %d\n",temp->onoma);
                    terminatedWorkers++;
                    if(terminatedWorkers==flag){
						/*otam termatisei ki o teleutaios worker*/
                        wasted_ups+=mysem_up(terminatingMain,__LINE__);
                    }
					wasted_ups+=mysem_up(waitingMain,__LINE__);
                    return(temp);
                }
				
				if(num != temp->number || temp->isready == 0){
					break;
				}
 				else{
					/*ksupna allon worker gt den einai etoimos*/
					wasted_ups+=mysem_up(workersQueue,__LINE__);
				}
			}
			
			mysem_down(mtx);
			/*worker douleuei- mh dia8esimos*/
            waitingWorkers--;
			if(waitingWorkers>0 && terminating!=-2 && terminating!=1){
				wasted_ups+=mysem_up(waitingMain,__LINE__);
			}
            wasted_ups+=mysem_up(mtx,__LINE__);
			
		}
		else{
			/*prvth fora pou trexei o worker*/
			isfirst=0;
		}
       
            
		gettimeofday(&t0, NULL);
    
		num=temp->number;
		
        mysem_down(mtx);
		times++;
		printf("->%d\n",times);
		temp->isprime = primetest(num);
		if(temp->isprime == 0){
			printf("(%d):   The number: %d is NOT prime!\n",temp->onoma, num);
		}
		else{
			printf("(%d):   The number: %d is prime!\n", temp->onoma, num);
		}
        gettimeofday(&t1, NULL);
        timersub(&t1, &t0, &dt);
        printf("Thread -%d-  took %d.%012d sec(number->(%d))\n\n", temp->onoma, (int)dt.tv_sec, (int)dt.tv_usec, num);

		waitingWorkers++;
        temp->isready = 1;
		if(waitingWorkers==1 && terminating==0){
			/*dinei shma oti enas worker einai pleon dia8esimos*/
			wasted_ups+=mysem_up(waitingMain,__LINE__);
		}
	}
}


int main(int argc, char* argv[]){
	int i, k, temp, num = atoi(argv[1]);
	int check, check_pthread, counter;
	struct informations *workers[num];
	pthread_t workersid[num];
    struct timeval k0, k1, dk;
     
    gettimeofday(&k0, NULL);
	
    waitingMain = mysem_create();
	if(waitingMain==-1){
		return(1);
	}
	terminatingMain = mysem_create();
	if(terminatingMain==-1){
		return(1);
	}
	mtx = mysem_create();
	if(mtx==-1){
		return(1);
	}
	workersQueue = mysem_create();
	if(workersQueue==-1){
		return(1);
	}
	
	mysem_down(waitingMain);
	mysem_down(terminatingMain);
	mysem_down(workersQueue);
	
	flag = 0;
	terminating = 0;
	terminatedWorkers = 0;
	waitingWorkers = 0;
    wasted_ups=0;
	times = 0;
	
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
		
		check_pthread = pthread_create(&workersid[i], NULL, primesearch, workers[i]);
		if(check_pthread != 0){
			printf("Error in pthread_create: %d\n", check_pthread);
			for(k=0; k<num; k++){
				free(workers[k]);
			}
			return(1);
		}
		printf("Main: Creation of worker[%d]=%lu\n",i,(long unsigned)workersid[i]);
	}
	
	counter=flag;
	
	while(1){
		check = scanf(" %d", &temp);
		if(check == EOF){
			break;
		}
		counter++;
		
		mysem_down(waitingMain);
		
		for(i=0; i<num; i++){
			if(workers[i]->isready==1){
				workers[i]->number = temp;
				workers[i]->isready = 0;
				workers[i]->isprime = -1;
				wasted_ups+=mysem_up(workersQueue,__LINE__);
				break;
			}
		}
	}
	

	/*perimenei mexri na teleiwsoun oloi oi workers*/
	printf("I'm waiting for workers to finish.\n");
	
	mysem_down(mtx);
	terminating=1;
	/*elegxos an exoun teleiwsei oloi oi workers*/
	if(waitingWorkers==flag){
		wasted_ups+=mysem_up(mtx,__LINE__);
	}
	else{
		/*perimenei na teleiwsoun oi ergates pou trexoun kai na dwsoun
		 * shma sth main*/
		wasted_ups+=mysem_up(mtx,__LINE__);
		mysem_down(waitingMain);
	}
	
	/*elegxei an uphrxan kapoioi teleutaioi ergates pou arghsan na 
	 * ksekinhsoun kai perimenei mexri na teleiwsoun ki autoi kai
	 * na steiloun shma sth main*/
	if(counter!=times){
		mysem_down(waitingMain);
	}
	
	terminating=-2;
	
	printf("I'm waiting for workers to terminate.\n");
	
	for(i=0; i<flag; i++){
        wasted_ups+=mysem_up(workersQueue,__LINE__);
		mysem_down(waitingMain);
    }
	
	mysem_down(terminatingMain);
	
	for(i=0;i<num;i++){        
        free(workers[i]);
    }

	gettimeofday(&k1, NULL);
    timersub(&k1, &k0, &dk);
        
    printf("Main took %d.%012d secs\n", (int)dk.tv_sec, (int)dk.tv_usec);
	printf("->%d\n",times);
    printf("WASTED UPS: %d\n",wasted_ups);
	return(0);
}
