/*karampinis ioannis 2529
 * kotsiaridis konstantinos 2547*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>
#include "mtx_cond.h"
#define COLOUR_RED "\x1b[31m"
#define COLOUR_BLUE "\x1b[34m"
#define RESET "\x1b[0m"

int bridgeCapacity, numCars;
int waitingBlue, waitingRed;
int inBridgeBlue, inBridgeRed, changePriority, passedCars;
pthread_mutex_t mtx;
pthread_cond_t go_blue, go_red;

void enter_bridge(char colour){
	
	if(colour == 'b'){
		mutex_lock(&mtx,__LINE__);
		/*ta amaksia prepei na perimenoun prin mpoun sth gefura*/
		if(inBridgeRed>0 || inBridgeBlue==bridgeCapacity || passedCars==changePriority){
			waitingBlue++;
			
			cond_wait(&go_blue,&mtx,__LINE__);
		
			passedCars++;
			if(inBridgeBlue==bridgeCapacity){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			}
			/*eidopoiei to epomeno amaksi*/
			if((waitingBlue>0)&&(inBridgeBlue<bridgeCapacity)&&(passedCars<changePriority)){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
				waitingBlue--;
				inBridgeBlue++;
				cond_signal(&go_blue,__LINE__);
			}
		}
		/*to amaksi mporei na perasei sth gefura kateu8eian*/
		else{
			inBridgeBlue++;
			passedCars++;
			/*eidopoiei to epomeno amaksi*/
			if((waitingBlue>0)&&(inBridgeBlue<bridgeCapacity)&&(passedCars<changePriority)){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
				waitingBlue--;
				inBridgeBlue++;
				cond_signal(&go_blue,__LINE__);
			}
			else{
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			}
		}
		mutex_unlock(&mtx,__LINE__);
	}
	/*ta idia pou kaname kai gia ta blue parapanw*/
	if(colour == 'r'){
		mutex_lock(&mtx,__LINE__);
		if(inBridgeBlue>0 || inBridgeRed==bridgeCapacity || passedCars==changePriority){
			waitingRed++;
			
			cond_wait(&go_red,&mtx, __LINE__);
			
			passedCars++;
			if(inBridgeRed==bridgeCapacity){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			}
			if((waitingRed>0)&&(inBridgeRed<bridgeCapacity)&&(passedCars<changePriority)){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
				inBridgeRed++;
				waitingRed--;
				cond_signal(&go_red,__LINE__);
			}
		}
		else{
			inBridgeRed++;
			passedCars++;
			if((waitingRed>0)&&(inBridgeRed<bridgeCapacity)&&(passedCars<changePriority)){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
				inBridgeRed++;
				waitingRed--;
				cond_signal(&go_red,__LINE__);
			}
			else{
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			}
		}
		mutex_unlock(&mtx,__LINE__);
	}
}
	
void exit_bridge(char colour){
	
	if(colour=='b'){
		mutex_lock(&mtx,__LINE__);
		printf(COLOUR_BLUE"------\n"RESET);
		inBridgeBlue--;
		/*den exei teleiwsei h proteraiothta twn blue || den perimenoun red*/
		if((waitingBlue>0 && inBridgeBlue<bridgeCapacity && passedCars<changePriority) || 
			(waitingBlue>0 && inBridgeBlue<bridgeCapacity && waitingRed==0)){
			printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			waitingBlue--;
			inBridgeBlue++;
			cond_signal(&go_blue,__LINE__);
		}
		/*allagh proteraiothtas*/
		else if(waitingRed>0 && inBridgeBlue==0){
			waitingRed--;
			inBridgeRed++;
			passedCars = 0;
			cond_signal(&go_red,__LINE__);
		}
		/*den perimenoun amaksia*/
		else if(waitingRed==0 && waitingBlue==0){
			passedCars=0;
		}
		mutex_unlock(&mtx,__LINE__);
	}
	/*akrivws ta idia pou kaname kai parapanw gia blue*/
	if(colour=='r'){
		mutex_lock(&mtx,__LINE__);
		printf(COLOUR_RED"------\n"RESET);
		inBridgeRed--;
		if((waitingRed>0 && inBridgeRed<bridgeCapacity && passedCars<changePriority) || 
			(waitingRed>0 && inBridgeRed<bridgeCapacity && waitingBlue==0)){
			printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			waitingRed--;
			inBridgeRed++;
			cond_signal(&go_red,__LINE__);
		}
		else if(waitingBlue>0 && inBridgeRed==0){
			waitingBlue--;
			inBridgeBlue++;
			passedCars = 0;
			cond_signal(&go_blue,__LINE__);
		}
		else if(waitingRed==0 && waitingBlue==0){
			passedCars=0;
		}
		mutex_unlock(&mtx,__LINE__);
	}
}


void *car(void *arg){
	char colour;
	
	colour= *(char*)arg;
	
	if(colour == 'b'){
		printf(COLOUR_BLUE"Trying to enter the bridge!\n"RESET);
		enter_bridge(colour);
		printf(COLOUR_BLUE"I AM ON THE BRIDGE!\n"RESET);
		sleep(1);
		exit_bridge(colour);
		printf(COLOUR_BLUE"I left the bridge!\n"RESET);
	}
	if(colour == 'r'){
		printf(COLOUR_RED"Trying to enter the bridge!\n"RESET);
		enter_bridge(colour);
		printf(COLOUR_RED"I AM ON THE BRIDGE!\n"RESET);
		sleep(1);
		exit_bridge(colour);
		printf(COLOUR_RED"I left the bridge!\n"RESET);
	}
	return(arg);
}

int main(int argc, char *argv[]){
	int  i, res, temp;
	char *colour;
	pthread_t *cars;
	
	if(argc!=1){
		printf("Wrong number of arguments. Termination!\n");
		return(1);
	}
	
	printf("Enter the capacity of the bridge: \n");
	scanf("%d", &bridgeCapacity);
	printf("Enter the total number of cars: \n");
	scanf("%d", &numCars);
	
	cars = (pthread_t*)malloc(sizeof(pthread_t)*numCars);
	if(cars==NULL){
		printf("Problem in malloc table cars.\n");
		return(1);
	}
	
	colour=(char*)malloc(sizeof(char)*numCars);
	if(colour==NULL){
		printf("Problem in malloc table colours.\n");
		return(1);
	}
	
	/*ari8mos cars apo ena xrwma pou epitrepetai na perasoun sunexomena*/
	changePriority = 2*bridgeCapacity;
	passedCars = 0; /*autokinhta apo ena xrwma pou perasan sunexomena*/
	inBridgeBlue = 0;
	inBridgeRed = 0;
	waitingBlue = 0;
	waitingRed = 0;
	temp = numCars;
	
	cond_init(&go_blue,__LINE__);
	cond_init(&go_red,__LINE__);
	mutex_init(&mtx,__LINE__);
	
	for(i=0; i<temp; i++){
		res = rand()%2;
		if(res ==1){
			colour[i] = 'b';
		}
		else{
			colour[i] = 'r';
		}
		
		res=pthread_create(&cars[i], NULL, car, &colour[i]);
		if(res!=0){
			printf("Pthread_create problem.\n");
			return(1);
		}
		printf("Thread created\n");
	}
	
	printf("--------------End of incoming CARS!----------\n");
	
	for(i=0;i<temp; i++){
		pthread_join(cars[i], NULL);
	}
	
	cond_destroy(&go_blue,__LINE__);
	cond_destroy(&go_red,__LINE__);
	mutex_destroy(&mtx,__LINE__);
		
	free(cars);
	free(colour);
	
	return(0);
}
