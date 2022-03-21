/*karampinis ioannis 2529
 * kotsiaridis konstantinos 2547*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

struct informations{
	int isprime;
	int isready;
	int number;
    int onoma;
};

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
	int num=-1;
	//pthread_t self=pthread_self();
	struct informations *temp;
    struct timeval t0, t1, dt;
	
	temp = (struct informations*)arg;
	
	printf("-----------------------Start of worker: %d---------------------------\n", temp->onoma);

	while(1){
		while(1){
			/*elegxei an do8ei neos ari8mos || dwsei shma h main*/
			if(num != temp->number || temp->isready == 0){
				break;
			}
		}
		gettimeofday(&t0, NULL);
    
		num=temp->number;
		
		/*termatismos twn threads*/
		if(num == -1){
			printf("Termination of thread: %d\n",temp->onoma);
			temp->isready = 1;
			return(temp);
		}
        
		temp->isprime = primetest(num);
		if(temp->isprime == 0){
			printf("(%d):   The number: %d is NOT prime!\n\n",temp->onoma, num);
		}
		else{
			printf("(%d):   The number: %d is prime!\n\n", temp->onoma, num);
		}
		temp->isready = 1;
        
        gettimeofday(&t1, NULL);
        timersub(&t1, &t0, &dt);
        
        printf("Thread -%d-  took %d.%012d sec(number->(%d))\n", temp->onoma, (int)dt.tv_sec, (int)dt.tv_usec, num);

	}
}


int main(int argc, char* argv[]){
	int i, k, temp, inloop, num = atoi(argv[1]);
	int check, check_pthread, flag=0;
	struct informations *workers[num];
	pthread_t workersid[num];
    struct timeval k0, k1, dk;
	
     
    gettimeofday(&k0, NULL);
    
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
	
	k=0;
	while(1){
		check = scanf(" %d", &temp);
		if(check == EOF){
			break;
		}
		inloop = 0;
		
		while(1){
			if(temp!=-1 && inloop == 1){
				k=0;
			}
			/*dinei douleia se opoion worker tuxei na brei eleu8ero prwto*/
			/*sunexizei meta apo ekei pou emeine*/
			for(i=k; i<num; i++){
				if(workers[i]->isready==1){
					workers[i]->number = temp;
					workers[i]->isready = 0;
					workers[i]->isprime = -1;
					temp = -1;
					break;
				}
			}
			if(temp==-1){/*diabazei epomeno xarakthra kai*/
				k=i+1;    /*sunexizei apo ekei pou stamathse*/
				if(k==num){
					k=0;
				}
				break;
			}
			else{/*arxise thn anazhthsh apo thn arxh tou pinaka*/
				inloop = 1;
			}
		}
	}
	
	
	/*perimenei mexri na teleiwsoun oloi oi workers*/
	for(i=0; i<flag; i++){
		printf("I'm waiting for worker %d to finish.\n", i);
		
		if(workers[i]->isready == 0){
			sleep(1);
			i--;
			continue;
		}
	}
	
	/*stelnei shma termatismou se olous tous workers*/
	for(i=0; i<flag; i++){
		printf("I command worker %d to terminate.\n", i);
		workers[i]->number= -1;
		workers[i]->isready = 0;
	}
	/*perimenei na termatistoun oloi oi workers*/
	for(i=0; i<flag; i++){
		printf("I'm waiting for worker %d to terminate.\n", i);
		if(workers[i]->isready == 0){
			sleep(1);
			i--;
			continue;
		}
	}
	
	for(i=0;i<num;i++){
		free(workers[i]);
	}
	
	gettimeofday(&k1, NULL);
    timersub(&k1, &k0, &dk);
        
    printf("Main took %d.%012d secs\n", (int)dk.tv_sec, (int)dk.tv_usec);
	
	return(0);
}
