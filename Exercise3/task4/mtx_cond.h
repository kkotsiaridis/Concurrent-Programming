#ifndef __MTX_COND_H__
#define __MTX_COND_H__

void mutex_init(pthread_mutex_t *mtx, int line);
void mutex_lock(pthread_mutex_t *mtx, int line);
void mutex_unlock(pthread_mutex_t *mtx, int line);
void mutex_destroy(pthread_mutex_t *mtx, int line);

void cond_init(pthread_cond_t *cond, int line);
void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mtx, int line);
void cond_signal(pthread_cond_t *cond, int line);
void cond_destroy(pthread_cond_t *cond, int line);

#endif
