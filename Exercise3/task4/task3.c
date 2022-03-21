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
#include "CCR.h"


CCR_DECLARE(treno);

int waitingPassengers, ridingPassengers, isriding;
int  trainCapacity,totalVisitors;


int enter(){
	
	int seat=0;
	
	CCR_EXEC(treno,1,\
		printf("Passenger: I'm waiting the train.\n");\
		waitingPassengers++;\
	);
	
	CCR_EXEC(treno,(ridingPassengers<trainCapacity && isriding==-1),\
		waitingPassengers--;\
		ridingPassengers++;\
		seat=ridingPassengers;\
		printf("I'm on the train!-%d/%d\n", ridingPassengers, trainCapacity);\
		if(ridingPassengers==trainCapacity){\
			isriding=-10;\
		}\
	);
	return(seat);
}


void *epivaths(void *arg){
	int seat=0;
	
	printf("Good morning!\n");
	seat=enter();
	CCR_EXEC(treno, (isriding==seat),\
		printf("I'm exiting the train.-%d/%d\n", ridingPassengers, trainCapacity);\
		ridingPassengers--;\
	);
	return(arg);
}


void *trenaki(void *arg){
	int i;
	
	isriding=-1; //arxizei h epibibash
	
	while(1){
		printf("------I'm waiting passengers to enter.------\n");
		CCR_EXEC(treno,(isriding==-10), );
		
		printf("------Beginning of the ride!------\n");
		sleep(4);
		printf("-----End of the ride!-----\n");

		for(i=0;i<=trainCapacity;i++){
			CCR_EXEC(treno,1,isriding=i;);
		}
		CCR_EXEC(treno,(ridingPassengers==0),isriding=-1;);
	}
}
	
	
int main(int argc, char *argv[]){
	int check, i;
	pthread_t t1, *visitors;
	
	printf("Enter train's capacity: \n");
	scanf(" %d", &trainCapacity);
	
	printf("Enter the number of visitors: \n");
	scanf(" %d", &totalVisitors);
	
	CCR_INIT(treno);
	
	waitingPassengers = 0; /*oi epivates pou dhmiourgh8hkan kai perimenoun*/
	ridingPassengers = 0; /*oi epivates pou epivivasthkan*/
	isriding = -2; 
	
	visitors = (pthread_t*)malloc(sizeof(pthread_t)*totalVisitors);
	if(visitors==NULL){
		printf("Problem in malloc!\n");
		return(1);
	}
	
	check = pthread_create(&t1, NULL, trenaki, NULL);
	if(check!=0){
		printf("Problem in creation of trenaki!\n");
		return(1);
	}
	
	for(i=0; i<totalVisitors; i++){
		check = pthread_create(&visitors[i], NULL, epivaths, NULL);
		if(check!=0){
			printf("Problem in creation of epivaths %d!\n", i);
			return(1);
		}
		sleep(1);
	}
	
	for(i=0; i<totalVisitors; i++){
		pthread_join(visitors[i], NULL);
	}
	pthread_join(t1, NULL);

	free(visitors);
	return(0);
}
