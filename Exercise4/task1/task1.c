/*karampinis ioannis 2529
 * kotsiaridis konstantinos 2547*/


#include "mycontext.h"
#include <string.h>

ucontext_t co_main, co_read, co_write;
int counter_read, counter_write, terminating, fd_in, fd_out;
char *table;

struct info{
	char name[20];
};

struct info *reading,*writing;

void initialize(){
	int i;
	
	for(i=0; i<SIZE; i++){
		table[i] = '\0';
	}
	
	return;
}

void pipe_close(){
	
	fprintf(stderr,"Close pipe\n");
	free(table);
	return;
}

void diff_func(){
	char cin, cout;
	off_t check;
	int check_in, check_out, counter=0;
	
	fprintf(stderr, "diff start\n");
	
	check = lseek(fd_in, 0, SEEK_SET);
	if(check==-1){
		fprintf(stderr,"Error in lseek in\n");
		return;
	}
	check = lseek(fd_out, 0, SEEK_SET);
	if(check==-1){
		fprintf(stderr,"Error in lseek out\n");
		return;
	}
	
	do{
		check_in = read(fd_in, &cin, 1);
		check_out = read(fd_out, &cout, 1);
		if(cin != cout){
			fprintf(stderr,"1-> %c, 2-> %c\n", cin, cout);
			fprintf(stderr,"not match\n");
			return;
		}
		counter++;
		//fprintf(stderr,"counter: %d\n", counter);
	}while(check_in!=0 && check_out!=0);
	
	if(check_in!=0 || check_out!=0){
		fprintf(stderr,"not match2\n");
		return;
	}
	
	fprintf(stderr,"diff end successfully\n");
	return;
}


void pipe_write(void *arg){
	int check;
	char character;
	struct info *curr;
	
	curr=(struct info *)arg;
	fprintf(stderr,"Open of the %s end of pipe\n",curr->name);
	
	while(1){
		while(counter_write != SIZE){
			check = scanf("%c", &character);
			if(check==EOF){
				terminating=1;
				return;
			}
			table[counter_write]=character;
			counter_write++;
		}
		fprintf(stderr,"Switch from write to read\n");
		mycoroutines_switchto(&co_write, &co_read);
		counter_write=0;
	}
}

void pipe_read(void *arg){
	int check;
	char character;
	struct info *curr;
	
	curr=(struct info *)arg;
	fprintf(stderr,"Open of the %s end of pipe\n",curr->name);
	
	while(1){
		while(counter_read!=counter_write){
			character = table[counter_read];
			counter_read++;
			check = write(STDOUT_FILENO, &character, 1);
			//fprintf(stderr,"%c",character);
			while(check!=1){
				check = write(STDOUT_FILENO, &character, 1);
			}
		}
		if(terminating==1){
			pipe_close();
			return;
		}
		else{
			counter_read=0;
			fprintf(stderr,"Switch from read to write\n");
			mycoroutines_switchto(&co_read, &co_write);
		}
	}
}
	


int main(int argc, char *argv[]){
	int check, oldin;
 	char file_in[STR_NAME], file_out[STR_NAME];
	
	strcpy(file_in, argv[1]);
	strcpy(file_out, argv[2]);
	fd_in = open(file_in, O_RDONLY, S_IRWXU);
	if(fd_in==-1){
		printf("Error in file_in opening\n");
		return(1);
	}
	
	fd_out = open(file_out, O_RDWR|O_CREAT, S_IRWXU);
	if(fd_out==-1){
		printf("Error in file_out opening\n");
		return(1);
	}
	
	oldin = dup(STDIN_FILENO);
	
	check = dup2(fd_in, STDIN_FILENO);
	if(check == -1){
		printf("Error in dup2-stdin\n");
		return(1);
	}
	
	check = dup2(fd_out, STDOUT_FILENO);
	if(check == -1){
		printf("Error in dup2-stdout\n");
		return(1);
	}
	
	table = (char*)malloc(SIZE*sizeof(char));
	if(table == NULL){
		fprintf(stderr,"Problem in malloc\n");
		return(1);
	}
	
	initialize();
	writing=(struct info *)malloc(sizeof(struct info));
	reading=(struct info *)malloc(sizeof(struct info));
	
	strcpy(reading->name,"reading");
	strcpy(writing->name,"writing");
	counter_read=0;
	counter_write=0;
	terminating = 0;
	
	check = mycoroutines_init(&co_main);
	if(check==-1){
		return(1);
	}
	
	
	check = mycoroutines_create(&co_write,pipe_write,writing,&co_read);
	if(check==-1){
		return(1);
	}
	check = mycoroutines_create(&co_read,pipe_read,reading,&co_main);
	if(check==-1){
		return(1);
	}
	
	check = mycoroutines_switchto(&co_main, &co_write);
	if(check==-1){
		return(1);
	}
	
	diff_func();
	
	close(fd_in);
	close(fd_out);
	close(oldin);
	
	mycoroutines_destroy(&co_read);
	mycoroutines_destroy(&co_write);
	mycoroutines_destroy(&co_main);

	return(0);
}
