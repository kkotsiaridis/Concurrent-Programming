#include "mtx_cond.h"

#define CCR_DECLARE(label) pthread_mutex_t mtx##label, mtx_q##label; \
                           pthread_cond_t cond_q##label;\
						   int num_q##label;\
						   int inloop##label;\
						   int enter##label;

#define CCR_INIT(label) mutex_init(&mtx##label, __LINE__);\
						mutex_init(&mtx_q##label, __LINE__);\
						cond_init(&cond_q##label,__LINE__);\
						num_q##label=0;\
						inloop##label=-1;\
						enter##label=0;
						
#define CCR_EXEC(label,cond,body) 	mutex_lock(&mtx##label,__LINE__);\
									mutex_lock(&mtx_q##label,__LINE__);\
									while ((!cond) || enter##label) {\
										num_q##label++;\
										if (inloop##label >= 0){\
											inloop##label++;\
											if(inloop##label==num_q##label||(inloop##label==num_q##label+1 && enter##label)){\
												inloop##label=-1;\
												enter##label=0;\
												mutex_unlock(&mtx##label,__LINE__);\
											}\
											else{\
												num_q##label--;\
												cond_signal(&cond_q##label,__LINE__);\
											}\
										}\
										else{\
											mutex_unlock(&mtx##label,__LINE__);\
										}\
										cond_wait(&cond_q##label,&mtx_q##label,__LINE__);\
									}\
									body;\
									if(num_q##label > 0){\
										num_q##label--;\
										if(inloop##label>=1){\
											enter##label=1;\
										}\
										else{\
											inloop##label=0;\
										}\
										cond_signal(&cond_q##label,__LINE__);\
										mutex_unlock(&mtx_q##label,__LINE__);\
									}\
									else{\
										inloop##label=-1;\
										mutex_unlock(&mtx_q##label,__LINE__);\
										mutex_unlock(&mtx##label,__LINE__);\
									}
