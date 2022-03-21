/*karampinis ioannis 2529
 * kotsiaridis konstantinos 2547*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define SIZE 64

int counter_read, counter_write, isclose;
char *table;

void initialize(){
	int i;
	
	for(i=0; i<SIZE; i++){
		table[i] = '\0';
	}
	
	return;
}

void pipe_close(){
    /*perimenei mexri na diabastei kai to teleutaio byte apo ton pinaka*/
	while(isclose!=-1){}
	fprintf(stderr,"Close pipe\n");
	free(table);
	return;
}

void pipe_write(char letter){
	
	table[counter_write] = letter;
	return;
}

int pipe_read(char *letter){
	
	*letter = table[counter_read];
	if((isclose == 1)&&(counter_read+1 == counter_write)){
		return(0);
	}
	else{
		return(1);
	}
}

void *thread_read(void *arg){
	char character;
	int check_piperead, check_write;
	
    /*perimenei mexri na arxisei h eggrafh xarakthrwn ston pinaka*/
	while(1){
		if(counter_write!=-1){
			break;
		}
	}
	
	while(1){
        /*elegxos an o counter_read einai sthn teleutaia 8esh toy pinaka*/
		if(counter_read+1 == SIZE){
            /*elegxos an o counter_write einai sthn epomenh 8esh,
             * wste na apofugoume kapoia prosperash*/
			if(counter_write!=0){
				counter_read = 0;
                /*anagnwsh xarakthra apo table*/
				check_piperead = pipe_read(&character);
				if(check_piperead == 0){
					sleep(1);
					isclose = -1;
					return(arg);
				}
				/*eggrafh xarakthra sthn eksodo*/
				check_write = write(STDOUT_FILENO, &character, 1);
				if(check_write ==-1){
					fprintf(stderr,"Write problem\n");
				}
				while(check_write == 0){
					check_write = write(STDOUT_FILENO, &character, 1);
				}
			}
		}
		else{
            /*elegxos an o counter_write einai sthn epomenh 8esh,
             * wste na apofugoume kapoia prosperash*/
			if(counter_read+1 != counter_write){
				counter_read++;
				check_piperead = pipe_read(&character);
				if(check_piperead == 0){
					sleep(1);
					isclose = -1;
					return(arg);
				}
				check_write = write(STDOUT_FILENO, &character, 1);
				if(check_write ==-1){
					fprintf(stderr,"Write problem\n");
				}
				while(check_write == 0){
					check_write = write(STDOUT_FILENO, &character, 1);
				}
			}
		}
	}
}


void *thread_write(void *arg){
	char character;
	int check;
	
	while(1){
		if(counter_write+1 == SIZE){
            /*elegxos an o counter_read einai sthn epomenh 8esh,
             * wste na apofugoume kapoia prosperash*/
			if(counter_read!=0){
				counter_write = 0;
				check = scanf("%c", &character);
				if(check == EOF){
					isclose = 1;
					counter_write++;
					return(arg);
				}
				/*eggrafh xarakthra sto table*/
				pipe_write(character);
			}
		}
		else{
            /*elegxos an o counter_read einai sthn epomenh 8esh,
             * wste na apofugoume kapoia prosperash*/
			if(counter_write+1 != counter_read){
				counter_write++;
				check = scanf("%c", &character);
				if(check == EOF){
					isclose = 1;
					counter_write++;
					return(arg);
				}
				pipe_write(character);
			}
		}
	}
}


int main(int argc, char *argv[]){
	int check;
	pthread_t t1,t2;
	
	table = (char*)malloc(SIZE*sizeof(char));
	if(table == NULL){
		printf("Problem in malloc\n");
		return(1);
	}
	
	counter_read=SIZE-1;
	counter_write=-1;
	isclose = 0;
	
	initialize();
	
	check = pthread_create(&t1, NULL, thread_read, NULL);
	if(check != 0){
		printf("Problem in pipe_read create\n");
		free(table);
		return(1);
	}
	
	check = pthread_create(&t2, NULL, thread_write, NULL);
	if(check != 0){
		printf("Problem in pipe_write create\n");
		free(table);
		return(1);
	}
	
	
	while(1){
        /*perimenei mexri na diabastei EOF*/
		if(isclose == 1){
			pipe_close();
			return(0);
		}	
	}
}
