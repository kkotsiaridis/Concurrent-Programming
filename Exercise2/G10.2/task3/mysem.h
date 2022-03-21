#ifndef __MYSEM_H__
#define __MYSEM_H__

int mysem_create();

void mysem_down(int semid);

int mysem_up(int semid, int line);

void mysem_destroy(int semid);

#endif
