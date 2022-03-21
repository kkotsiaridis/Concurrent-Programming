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

int waitingPassengers, ridingPassengers, isriding, totalVisitors, noRide;
int train, passengers, trainCapacity, waitMain, intrain, mtx, getDown;

void enter(){
	
	mysem_down(mtx);
	/*o epivaths prepei na perimenei*/
	if(isriding!=0 || ridingPassengers==trainCapacity){
		waitingPassengers++;
		mysem_up(mtx,__LINE__);
		printf("Passenger: I'm waiting the train.\n");
		mysem_down(passengers);
		printf("I'm on the train!-%d/%d\n", ridingPassengers, trainCapacity);
		mysem_up(mtx,__LINE__);
	}
	else{
		/*o epivaths hr8e, to treno den taksideue kai xwrouse*/
		ridingPassengers++;
		printf("I'm on the train!-%d/%d\n", ridingPassengers, trainCapacity);
		mysem_up(mtx,__LINE__);
	}
	
	mysem_down(mtx);
	if(ridingPassengers<trainCapacity && waitingPassengers>0){
		waitingPassengers--;
		ridingPassengers++;
		/*eidopoiei ton epomeno epivath oti mporei na epivivastei*/
		mysem_up(passengers,__LINE__);
	}
	else{
		mysem_up(mtx,__LINE__);
	}
	
	mysem_down(mtx);
	totalVisitors--;
	/*elegxei an to treno gemise*/
	if(((totalVisitors%trainCapacity)-noRide)==0){
		if(ridingPassengers==trainCapacity){
			/*to treno ksekina*/
			isriding=1;
			mysem_up(train,__LINE__);
		}
	}
	mysem_up(mtx,__LINE__);
}

void apovivash(){
	for(;ridingPassengers>0; ridingPassengers--){
		/*dinei shma ston epivath na apovivastei*/
		mysem_up(intrain,__LINE__);
		/*perimenei na apovivastei*/
		mysem_down(getDown);
	}
}

void *trenaki(void *arg){
	while(1){
		mysem_down(mtx);
		if(waitingPassengers>0 && ridingPassengers<trainCapacity){
			waitingPassengers--;
			ridingPassengers++;
			/*dinei shma ston prwto epivath na epivivastei*/
			mysem_up(passengers,__LINE__);
		}
		else{
			mysem_up(mtx,__LINE__);
		}
		
		printf("------I'm waiting passengers to enter.------\n");
		/*to treno perimenei na gemisei*/
		mysem_down(train);
		
		printf("------Beginning of the ride!------\n");
		sleep(5);
		printf("-----End of the ride!-----\n");
		
		apovivash();
		
		mysem_down(mtx);
		/*to treno teleiwse thn diadromh ki einai dia8esimo*/
		isriding=0;
		/*sun8hkh termatismou an xreiastei mesa sta sxolia*/
		/*if(totalVisitors==0){
			mysem_up(waitMain,__LINE__);
			mysem_up(mtx,__LINE__);
			return(arg);
		}*/
		mysem_up(mtx,__LINE__);
	}
}

void *epivaths(void *arg){
	printf("Good morning!\n");
	enter();
	mysem_down(intrain);
	printf("I'm exiting the train.\n");
	mysem_up(getDown,__LINE__);
	
	return(arg);
}
	
	
int main(int argc, char *argv[]){
	int check, i, counter;
	pthread_t t1, *visitors;
	
	printf("Enter train's capacity: \n");
	scanf(" %d", &trainCapacity);
	
	printf("Enter the number of visitors: \n");
	scanf(" %d", &totalVisitors);
	
	train=mysem_create();
	if(train==-1){
		return(1);
	}
	passengers=mysem_create();
	if(passengers==-1){
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
	intrain=mysem_create();
	if(intrain==-1){
		return(1);
	}
	getDown=mysem_create();
	if(getDown==-1){
		return(1);
	}
	
	mysem_down(intrain);
	mysem_down(waitMain);
	mysem_down(train);
	mysem_down(passengers);
	mysem_down(getDown);
	
	waitingPassengers = 0; /*oi epivates pou dhmiourgh8hkan kai perimenoun*/
	ridingPassengers = 0; /*oi epivates pou epivivasthkan*/
	isriding = 0; /*0-> den einai se didromh to treno*/
	counter=totalVisitors;
	noRide = totalVisitors%trainCapacity; /*epivates pou perisseuoun kai den
											8a taksidepsoun*/
	
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
	
	for(i=0; i<counter; i++){
		check = pthread_create(&visitors[i], NULL, epivaths, NULL);
		if(check!=0){
			printf("Problem in creation of epivaths %d!\n", i);
			return(1);
		}
		sleep(1);
	}
	
	mysem_down(waitMain);
	free(visitors);
	return(0);
}
