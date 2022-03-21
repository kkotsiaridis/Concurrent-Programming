#include <stdio.h>
#include <pthread.h>
#include <string.h>

void mutex_init(pthread_mutex_t *mtx, int line){
	pthread_mutexattr_t attr;
	int res;
	
	res = pthread_mutexattr_init(&attr);
	if(res!=0){
		printf("Line: %d - Error in mutex_init: pthread_mutexattr_init - %s\n",line,strerror(res));
		return;
	}
	
	res = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	if(res!=0){
		printf("Line: %d - Error in mutex_init: pthread_mutexattr_settype - %s\n",line,strerror(res));
		return;
	}
	
	res = pthread_mutex_init(mtx, &attr);
	if(res!=0){
		printf("Line: %d - Error in mutex_init: pthread_mutex_init - %s\n",line,strerror(res));
		return;
	}
	
	res = pthread_mutexattr_destroy(&attr);
	if(res!=0){
		printf("Line: %d - Error in mutex_init: pthread_mutexattr_destroy - %s\n",line,strerror(res));
		return;
	}
	
	return;
}


void mutex_lock(pthread_mutex_t *mtx, int line){
	int res;
	
	res = pthread_mutex_lock(mtx);
	if(res!=0){
		printf("Line: %d - Error in pthread_mutex_lock - %s\n",line,strerror(res));
		return;
	}
	
	return;
}

void mutex_unlock(pthread_mutex_t *mtx, int line){
	int res;
	
	res = pthread_mutex_unlock(mtx);
	if(res!=0){
		printf("Line: %d - Error in pthread_mutex_unlock - %s\n",line,strerror(res));
		return;
	}
	
	return;
}

void mutex_destroy(pthread_mutex_t *mtx, int line){
	int res;
	
	res = pthread_mutex_destroy(mtx);
	if(res!=0){
		printf("Line: %d - Error in pthread_mutex_destroy- %s\n",line, strerror(res));
		return;
	}
	
	return;
}


void cond_init(pthread_cond_t *cond, int line){
	int res;
	
	res = pthread_cond_init(cond,NULL);
		if(res!=0){
		printf("Line: %d - Error in pthread_cond_init - %s\n",line,strerror(res));
		return;
	}
	
	return;
}

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mtx, int line){
	int res;
	
	res = pthread_cond_wait(cond,mtx);
		if(res!=0){
		printf("Line: %d - Error in pthread_cond_wait - %s\n",line,strerror(res));
		return;
	}
	
	return;
}

void cond_signal(pthread_cond_t *cond, int line){
	int res;
	
	res = pthread_cond_signal(cond);
		if(res!=0){
		printf("Line: %d - Error in pthread_cond_signal - %s\n",line,strerror(res));
		return;
	}
	
	return;
}

void cond_destroy(pthread_cond_t *cond, int line){
	int res;
	
	res = pthread_cond_destroy(cond);
		if(res!=0){
		printf("Line: %d - Error in pthread_cond_destroy - %s\n",line,strerror(res));
		return;
	}
	
	return;
}
