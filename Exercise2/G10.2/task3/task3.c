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
#include "mysem.h"
#define COLOUR_RED "\x1b[31m"
#define COLOUR_BLUE "\x1b[34m"
#define RESET "\x1b[0m"

int bridgeCapacity, numCars;
int waitingBlue, waitingRed;
int inBridgeBlue, inBridgeRed;
int go_blue, go_red, mtx; 
int waitMain, changePriority, passedCars;

void enter_bridge(char colour){
	
	if(colour == 'b'){
		mysem_down(mtx);
		/*ta amaksia prepei na perimenoun prin mpoun sth gefura*/
		if(inBridgeRed>0 || inBridgeBlue==bridgeCapacity || passedCars==changePriority){
			waitingBlue++;
			mysem_up(mtx,__LINE__);
			mysem_down(go_blue);
			mysem_down(mtx);
			passedCars++;
			if(inBridgeBlue==bridgeCapacity){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			}
			/*eidopoiei to epomeno amaksi*/
			if((waitingBlue>0)&&(inBridgeBlue<bridgeCapacity)&&(passedCars<changePriority)){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
				waitingBlue--;
				inBridgeBlue++;
				mysem_up(go_blue,__LINE__);
			}
		}
		/*to amaksi mporei na erasei sth gefura kateu8eian*/
		else{
			inBridgeBlue++;
			passedCars++;
			/*eidopoiei to epomeno amaksi*/
			if((waitingBlue>0)&&(inBridgeBlue<bridgeCapacity)){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
				waitingBlue--;
				inBridgeBlue++;
				mysem_up(go_blue,__LINE__);
			}
			else{
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			}
		}
		mysem_up(mtx,__LINE__);
	}
	/*ta idia pou kaname kai gia ta blue parapanw*/
	if(colour == 'r'){
		mysem_down(mtx);
		if(inBridgeBlue>0 || inBridgeRed==bridgeCapacity || passedCars==changePriority){
			waitingRed++;
			mysem_up(mtx,__LINE__);
			mysem_down(go_red);
			mysem_down(mtx);
			passedCars++;
			if(inBridgeRed==bridgeCapacity){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			}
			if((waitingRed>0)&&(inBridgeRed<bridgeCapacity)&&(passedCars<changePriority)){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
				inBridgeRed++;
				waitingRed--;
				mysem_up(go_red,__LINE__);
			}
		}
		else{
			inBridgeRed++;
			passedCars++;
			if((waitingRed>0)&&(inBridgeRed<bridgeCapacity)){
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
				inBridgeRed++;
				waitingRed--;
				mysem_up(go_red,__LINE__);
			}
			else{
				printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			}
		}
		mysem_up(mtx,__LINE__);
	}
}
	
void exit_bridge(char colour){
	
	if(colour=='b'){
		mysem_down(mtx);
		printf(COLOUR_BLUE"------\n"RESET);
		inBridgeBlue--;
		/*den exei teleiwsei h proteraiothta twn blue || den perimenoun red*/
		if((waitingBlue>0 && inBridgeBlue<bridgeCapacity && passedCars<changePriority) || 
			(waitingBlue>0 && inBridgeBlue<bridgeCapacity && waitingRed==0)){
			printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			waitingBlue--;
			inBridgeBlue++;
			mysem_up(go_blue,__LINE__);
		}
		/*allagh proteraiothtas*/
		else if(waitingRed>0 && inBridgeBlue==0){
			waitingRed--;
			inBridgeRed++;
			passedCars = 0;
			mysem_up(go_red,__LINE__);
		}
		/*den perimenoun amaksia*/
		else if(waitingRed==0 && waitingBlue==0){
			passedCars=0;
		}
		mysem_up(mtx,__LINE__);
	}
	/*akrivws ta idia pou kaname kai parapanw gia blue*/
	if(colour=='r'){
		mysem_down(mtx);
		printf(COLOUR_RED"------\n"RESET);
		inBridgeRed--;
		if((waitingRed>0 && inBridgeRed<bridgeCapacity && passedCars<changePriority) || 
			(waitingRed>0 && inBridgeRed<bridgeCapacity && waitingBlue==0)){
			printf("inR: %d, inB: %d, wR: %d, wB: %d\n", inBridgeRed, inBridgeBlue, waitingRed, waitingBlue);
			waitingRed--;
			inBridgeRed++;
			mysem_up(go_red,__LINE__);
		}
		else if(waitingBlue>0 && inBridgeRed==0){
			waitingBlue--;
			inBridgeBlue++;
			passedCars = 0;
			mysem_up(go_blue,__LINE__);
		}
		else if(waitingRed==0 && waitingBlue==0){
			passedCars=0;
		}
		mysem_up(mtx,__LINE__);
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
	
	/*sun8hkh termatismou ths gefuras an xreiastei*/
	/*numCars--;	
	if(numCars==0){
		mysem_up(waitMain);
	}*/
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
	
	go_blue=mysem_create();
	if(go_blue==-1){
		return(1);
	}
	go_red=mysem_create();
	if(go_red==-1){
		return(1);
	}
	waitMain=mysem_create();
	if(waitMain==-1){
		return(1);
	}
	mtx=mysem_create();
	if(mtx==-1){
		return(1);
	}
	mysem_down(waitMain);
	mysem_down(go_red);
	mysem_down(go_blue);
	
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
	mysem_down(waitMain);
	
	mysem_destroy(go_blue);
	mysem_destroy(go_red);
	mysem_destroy(waitMain);
	mysem_destroy(mtx);
		
	free(cars);
	free(colour);
	
	return(0);
}
