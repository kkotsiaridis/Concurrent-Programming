/*karampinis ioannis 2529
 * kotsiaridis konstantinos 2547*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mtx_cond.h"

struct informations{
	int isprime;
	int isready;
	int number;
    int onoma;
};

pthread_mutex_t mtx;
pthread_cond_t workersQueue, waitingMain, terminatingMain;
int flag, waitingWorkers, times,terminatedWorkers;

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
	
	temp = (struct informations*)arg;
	
	printf("-----------------------Start of worker: %d---------------------------\n", temp->onoma);
     
	while(1){
		if(isfirst!=1){
			while(1){
				cond_wait(&workersQueue, &mtx, __LINE__);
				if(temp->number==-1){
					printf("Termination of thread: %d\n",temp->onoma);
					terminatedWorkers++;
					if(terminatedWorkers==flag){      /*otam termatisei ki o teleutaios worker*/
						cond_signal(&terminatingMain,__LINE__);
					}
					mutex_unlock(&mtx,__LINE__);
					return(temp);
				}
				if(temp->isready==0){
					mutex_unlock(&mtx,__LINE__);
					break;
				}
				else{
					/*se periptwsh pou uparksei prosperash kai parei o lathos worker douleia*/
					cond_signal(&workersQueue,__LINE__);
				}
			}
		}
		else{
			isfirst=0;  /*prvth fora pou trexei o worker*/
		}
    
		num=temp->number;
		
		temp->isprime = primetest(num);
		
		if(temp->isprime == 0){
			printf("(%d):   The number: %d is NOT prime!\n",temp->onoma, num);
		}
		else{
			printf("(%d):   The number: %d is prime!\n", temp->onoma, num);
		}
	
		mutex_lock(&mtx, __LINE__);
		times++;
		waitingWorkers++;
        temp->isready = 1;
		if(waitingWorkers==1){
			cond_signal(&waitingMain,__LINE__);    /*dinei shma oti enas worker einai pleon dia8esimos*/
		}
	}
}


int main(int argc, char* argv[]){
	int i, k, temp, num = atoi(argv[1]);
	int check, check_pthread;
	struct informations *workers[num];
	pthread_t workersid[num];
    struct timeval k0, k1, dk;
     
    gettimeofday(&k0, NULL);
	
    cond_init(&waitingMain, __LINE__);
	cond_init(&terminatingMain, __LINE__);
	cond_init(&workersQueue, __LINE__);
	mutex_init(&mtx, __LINE__);
	
	flag = 0;
	terminatedWorkers = 0;
	waitingWorkers = 0;
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
	
	
	while(1){
		check = scanf(" %d", &temp);
		if(check == EOF){
			break;
		}
		
		mutex_lock(&mtx,__LINE__);
		if(waitingWorkers==0){
			cond_wait(&waitingMain, &mtx, __LINE__);
		}
		mutex_unlock(&mtx, __LINE__);
		mutex_lock(&mtx, __LINE__);
		for(i=0; i<num; i++){
			if(workers[i]->isready==1){
				workers[i]->number = temp;
				workers[i]->isready = 0;
				workers[i]->isprime = -1;
				waitingWorkers--;
				cond_signal(&workersQueue,__LINE__);
				mutex_unlock(&mtx, __LINE__);
				break;
			}
		}
	}
	
	printf("I'm waiting for workers to finish.\n");
	/*ka8e worker pou teleiwnei tou stelnoume to -1 gia na termatisei*/
	for(k=0;k<flag;k++){
	
		mutex_lock(&mtx,__LINE__);
		if(waitingWorkers==0){
			cond_wait(&waitingMain, &mtx, __LINE__);
		}
		mutex_unlock(&mtx, __LINE__);
		mutex_lock(&mtx, __LINE__);
		for(i=0; i<flag; i++){
			if(workers[i]->isready==1){
				workers[i]->number = -1;
				workers[i]->isready = 0;
				workers[i]->isprime = -1;
				waitingWorkers--;
				cond_signal(&workersQueue,__LINE__);
				mutex_unlock(&mtx, __LINE__);
				break;
			}
		}
	}
	
	
	printf("I'm waiting for workers to terminate.\n");
	
	mutex_lock(&mtx,__LINE__);
	/*xwris thn sunthkiki ayth kollane sto wait oi 2 periptwseis
	 * signal blocking*/
	if(terminatedWorkers<flag){
		cond_wait(&terminatingMain,&mtx, __LINE__);
	}
	mutex_unlock(&mtx, __LINE__);
	
	for(i=0;i<flag;i++){        
        free(workers[i]);
    }

	gettimeofday(&k1, NULL);
    timersub(&k1, &k0, &dk);
    printf("Main took %d.%012d secs\n", (int)dk.tv_sec, (int)dk.tv_usec);
	printf("->%d\n",times);
	
	cond_destroy(&terminatingMain,__LINE__);
	cond_destroy(&waitingMain,__LINE__);
	cond_destroy(&workersQueue,__LINE__);
	mutex_destroy(&mtx,__LINE__);
	
	return(0);
}
