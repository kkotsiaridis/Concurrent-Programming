/*karampinis ioannis 2529
 * kotsiaridis konstantinos 2547*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

struct informations{
	int isready;
	int *table;
	int first,last;
	int level;
};

void *quicksort(void * arg){
	struct informations *info_left,*info_right, *local;
	int first, last, i, j, k, s_diaxwrismou, temp, check; 
	pthread_t thread_left, thread_right, self=pthread_self();
	
	local=(struct informations*)arg;
	
	first=local->first;
	last=local->last;
	local->level++;
	
	printf("Creation of thread(%lu)-level(%d):", self, local->level);
	if(first<last){
		/*ektupwnei to kommati tou pinaka sto opoio douleuei to thread*/
		for(k=first; k<=last; k++){
			printf("%d_", local->table[k]);
		}
		printf("\n");
	}
	else if(first==last){
		/*an den mporei na diaspastei allo to kommati tou pinaka*/
		printf(" |%d|\n", local->table[first]);
	}
	else{
		/*sthn periptwsh pou deksia tou stoixeiou diaxwrismou den
		 * uparxoun stoixeia*/
		printf("-\n");
	}
	
	if(first<last){
		s_diaxwrismou=first;
		i=first;
		j=last;

		while(i<j){
			while(local->table[i]<=local->table[s_diaxwrismou]&&i<last){
				i++;
			}
			while(local->table[j]>local->table[s_diaxwrismou]){
				j--;
			}
			if(i<j){
				temp=local->table[i];
				local->table[i]=local->table[j];
				local->table[j]=temp;
			}
		}
		
		temp=local->table[s_diaxwrismou];
		local->table[s_diaxwrismou]=local->table[j];
		local->table[j]=temp;
		
		info_left =(struct informations*)malloc(sizeof(struct informations));
		if(info_left == NULL){
			printf("Problem in malloc-left: %lu\n", self);
			local->isready = 1;
			return(local);
		}
		
		info_left->table = local->table;
		info_left->isready=0;
		info_left->first=local->first;
		info_left->last=j;
		info_left->level=local->level;
		
		info_right =(struct informations*)malloc(sizeof(struct informations));
		if(info_right == NULL){
			printf("Problem in malloc-right: %lu\n", self);
			free(info_left);
			local->isready = 1;
			return(local);
		}
		
		info_right->table = local->table;
		info_right->isready=0;
		info_right->first=j+1;
		info_right->last=local->last;
		info_right->level=local->level;
		
		
		check = pthread_create(&thread_left, NULL, quicksort, info_left);
		if(check!=0){
			printf("Pthread_create_left: %lu\n", self);
			local->isready = 1;
			return(local);
		}
		
		check = pthread_create(&thread_right, NULL, quicksort, info_right);
		if(check!=0){
			printf("Pthread_create_right: %lu\n", self);
			local->isready = 1;
			return(local);
		}
	}
	else{
		/*sthn periptwsh pou exoume ftasei se ena stoixeiou
		 * ki o pinakas den diaspate allo*/
		printf("End thread: %lu\n",self);
		sleep(1);
		local->isready=1;
		return(local);
	}
   
	while(1){
		/*perimenei mexri na teleiwsoun ta duo tmhmata sta opoia eixe 
		 * spasei auto to kommati tou pinaka*/
		if((info_left->isready == 1)&&(info_left->isready == 1)){
			free(info_left);
			free(info_right);
			printf("End thread: %lu\n",self);
			
			local->isready=1;
			return(local);
		}
	}
}


int main(int argc, char *argv[]){
	int counter, check, i, temp;
	int *table;
	pthread_t thread;
	struct informations *info;
	
	table=(int*)malloc(sizeof(int));
	if(table == NULL){
		printf("Malloc in main!\n");
		return(1);
	}
	
	scanf(" %d",table);
	counter=1;
	while(1){
		check=scanf(" %d",&temp);	
		if(check == EOF){
			break;
		}
		/*upologizei to plh8os twn ari8mwn pros diataksh*/
		counter++;
		/*auksanei to mege8os tou pinaka katallhla*/
		table=(int*)realloc(table,counter * sizeof(int));
		if(table ==NULL){
			printf("Realloc-%d in main\n",counter);
			return(1);
		}
		table[counter-1]=temp;
	}
	
	info=(struct informations*)malloc(sizeof(struct informations));
	if(info == NULL){
		printf("Malloc of struct in main\n");
		return(1);
	}
	info->table=table;
	info->isready=0;
	info->last=counter-1;
	info->first=0;
	info->level = 0;
	check= pthread_create(&thread, NULL, quicksort,info);
	if(check != 0){
		printf("Pthread_create error in main!\n");
		return(1);
	}
	
	while(1){
		sleep(1);
		/*perimenei mexri na teleiwsei h anadromh kai na epistrepsei
		 * h takshnomhmenh morfh tou pinaka*/
		if(info->isready == 1){
			for(i=0;i<counter;i++){
				printf("%d/",info->table[i]);
			}
			printf("\n");
			
			free(table);
			free(info);
			return(0);
		}
	}
}
