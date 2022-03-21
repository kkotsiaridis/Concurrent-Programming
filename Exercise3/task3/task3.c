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

int waitingPassengers, ridingPassengers, isriding;
int  trainCapacity, totalVisitors, noRide, leave;
pthread_mutex_t mtx;
pthread_cond_t waitMain, train, passengers, intrain, getDown;


void enter(){
	
	mutex_lock(&mtx,__LINE__);
	/*o epivaths prepei na perimenei*/
	if(isriding!=0 || ridingPassengers==trainCapacity){
		waitingPassengers++;
		printf("Passenger: I'm waiting the train.\n");
		cond_wait(&passengers,&mtx,__LINE__);
	}
	else{
		/*o epivaths hr8e, to treno den taksideue kai xwrouse*/
		ridingPassengers++;
		printf("I'm on the train!-%d/%d\n", ridingPassengers, trainCapacity);
	}
	
	if(ridingPassengers<trainCapacity && waitingPassengers>0){
		waitingPassengers--;
		ridingPassengers++;
		/*eidopoiei ton epomeno epivath oti mporei na epivivastei*/
		cond_signal(&passengers,__LINE__);
		printf("I'm on the train!-%d/%d\n", ridingPassengers, trainCapacity);
	}
	
	totalVisitors--;
	/*elegxei an to treno gemise*/
	if(((totalVisitors%trainCapacity)-noRide)==0){
		if(ridingPassengers==trainCapacity){
			/*to treno ksekina*/
			isriding=1;
			cond_signal(&train,__LINE__);
		}
	}
}

void *epivaths(void *arg){
	printf("Good morning!\n");
	enter();
	cond_wait(&intrain,&mtx,__LINE__);
	printf("I'm exiting the train.\n");
	leave++;
	if(leave==trainCapacity){  //gia signal blocking
		cond_signal(&getDown,__LINE__);
	}
	mutex_unlock(&mtx,__LINE__);
	return(arg);
}

void apovivash(){
	
	mutex_lock(&mtx,__LINE__);
	for(;ridingPassengers>0; ridingPassengers--){
		/*dinei shma ston epivath na apovivastei*/
		cond_signal(&intrain,__LINE__);
	}
	/*h sunthiki boh8a gia tis signal blocking*/
	if(leave!= trainCapacity){
		cond_wait(&getDown,&mtx,__LINE__);
	}
	leave=0;
	mutex_unlock(&mtx,__LINE__);
}

void *trenaki(void *arg){
	int sleeping=0;
	
	while(1){
		mutex_lock(&mtx,__LINE__);
		isriding=0;
		//for gia olous tous waitingPassengers gia dikaih oyra anamonhs
		if(waitingPassengers>0 && ridingPassengers<trainCapacity){
			waitingPassengers--;
			ridingPassengers++;
			sleeping=1;
			/*dinei shma ston prwto epivath na epivivastei*/
			cond_signal(&passengers,__LINE__);
		}
		
		printf("------I'm waiting passengers to enter.------\n");
		/*to treno perimenei na gemisei*/
		if(sleeping==1){
			printf("I'm on the train!-%d/%d\n", ridingPassengers, trainCapacity);
			sleeping=0;
		}
		cond_wait(&train,&mtx, __LINE__);
		mutex_unlock(&mtx,__LINE__);
		
		printf("------Beginning of the ride!------\n");
		sleep(5);
		printf("-----End of the ride!-----\n");
		
		apovivash();
	}
}
	
	
int main(int argc, char *argv[]){
	int check, i, counter;
	pthread_t t1, *visitors;
	
	printf("Enter train's capacity: \n");
	scanf(" %d", &trainCapacity);
	
	printf("Enter the number of visitors: \n");
	scanf(" %d", &totalVisitors);
	
	cond_init(&train,__LINE__);
	cond_init(&passengers,__LINE__);
	cond_init(&intrain,__LINE__);
	cond_init(&getDown,__LINE__);
	mutex_init(&mtx,__LINE__);
	
	waitingPassengers = 0; /*oi epivates pou dhmiourgh8hkan kai perimenoun*/
	ridingPassengers = 0; /*oi epivates pou epivivasthkan*/
	isriding = -1; /*0-> den einai se didromh to treno*/
	counter=totalVisitors;
	leave=0;
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
	
	for(i=0; i<counter; i++){
		pthread_join(visitors[i], NULL);
	}
	pthread_join(t1, NULL);

	cond_destroy(&train,__LINE__);
	cond_destroy(&intrain,__LINE__);
	cond_destroy(&getDown,__LINE__);
	cond_destroy(&passengers,__LINE__);
	
	mutex_destroy(&mtx,__LINE__);

	free(visitors);
	return(0);
}
