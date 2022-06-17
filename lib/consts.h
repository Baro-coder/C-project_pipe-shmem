
#ifndef _CONSTS_H

#define _CONSTS_H

#include <sys/shm.h>
#include <semaphore.h>

char * p1_src_file = "./var/p1.out";
char * p2_src_file = "./var/p2.out";
char * p3_src_file = "./var/p3.out";

#define BUFFSIZE 128

#define READ 0
#define WRITE 1

#define SHMEM_KEY_CODE 1234

struct shmem_seg_struct
{
    sem_t mutex_2;
    sem_t mutex_3;
    char data[BUFFSIZE];
};

#endif