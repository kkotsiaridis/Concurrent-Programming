#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int mysem_create(){
	int check, semid;
	
	semid = semget(IPC_PRIVATE, 2,S_IRWXU);
	if(semid==-1){
		printf("ERROR in mysem_create: semget- %s\n", strerror(semid));
		return(-1);
	}
	
	check = semctl(semid, 0, SETVAL, 1);
	if(check==-1){
		printf("ERROR in mysem_create: semctl- %s\n", strerror(check));
		return(-1);
	}
	
	check = semctl(semid, 1, SETVAL, 1);
	if(check==-1){
		printf("ERROR in mysem_create mtx: semctl- %s\n", strerror(check));
		return(-1);
	}
	
	return(semid);
}

void mysem_down(int semid){
	int check,sem_value;
	struct sembuf op,op1;
	
	op.sem_num = 1;
	op.sem_op = -1;
	op.sem_flg = 0;
	
	check = semop(semid, &op, 1);//down mtx
	if(check==-1){
		printf("ERROR in mysem_down: semop- %s\n", strerror(check));
		return;
	}
	sem_value = semctl(semid, 0, GETVAL);//getval sem 
	if(sem_value==-1){
		printf("ERROR in mysem_up: semctl- %s\n", strerror(sem_value));
		return;
	}
	
	op.sem_num = 0;
	op.sem_op = -1;
	op.sem_flg = 0;
	
	op1.sem_num = 1;
	op1.sem_op = 1;
	op1.sem_flg = 0;
	
	if(sem_value==0){//elegxos gia up_mtx prin down_sem den otan stamataei to thread
		check = semop(semid, &op1, 1);
		if(check==-1){
			printf("ERROR in mysem_down: semop- %s\n", strerror(check));
			return;
		}
	}
	
	check = semop(semid, &op, 1);//down sem
	if(check==-1){
		printf("ERROR in mysem_down: semop- %s\n", strerror(check));
		return;
	}
	
	if(sem_value==1){
		check = semop(semid, &op1, 1);//up_mtx an den exei stamathsei sto down 
		if(check==-1){
			printf("ERROR in mysem_down: semop- %s\n", strerror(check));
			return;
		}
	}
	
	return;
}


int mysem_up(int semid, int line){
	int check, sem_value;
	struct sembuf op;
	
	op.sem_num = 1;
	op.sem_op = -1;
	op.sem_flg = 0;
	
	check = semop(semid, &op, 1);//down mtx
	if(check==-1){
		printf("ERROR in mysem_up-down_mtx: semop- %s\n", strerror(check));
		return(-1);
	}
	
	sem_value = semctl(semid, 0, GETVAL);//getval sem
	if(sem_value==-1){
		printf("ERROR in mysem_up: semctl- %s\n", strerror(sem_value));
		return(-1);
	}
	
	if(sem_value==1){
		printf("Wasted system call sem_up!-------> %d\n", line);//print wasted
		op.sem_num = 1;
		op.sem_op = 1;
		
		check = semop(semid, &op, 1);//up mtx
		if(check==-1){
			printf("ERROR in mysem_up-down_mtx: semop- %s\n", strerror(check));
			return(-1);
		}
		
		return(1);
	}
	
	op.sem_num = 0;
	op.sem_op = 1;
	
	check = semop(semid, &op, 1);//up sem
	if(check==-1){
		printf("ERROR in mysem_up: semop- %s\n", strerror(check));
		return(-1);
	}
	
	op.sem_num = 1;
	
	check = semop(semid, &op, 1);//up mtx
	if(check==-1){
		printf("ERROR in mysem_up-down_mtx: semop- %s\n", strerror(check));
		return(-1);
	}

	return(0);
}
	
	
void mysem_destroy(int semid){
	int check;
	
	check = semctl(semid, 0,IPC_RMID);
	if(check==-1){
		printf("ERROR in mysem_destroy: semctl- %s\n", strerror(check));
		return;
	}
	
	return;
}
