// LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>

// HEADERS
#include "consts.h"

// FUNCTIONS
void build();
void sigHandler(int signo);
void asciiToHexConvert(char * input, char * output);

int p_num;  // Process number

// PIPE
int pipe_fd[2];     // Pipe: P1 -> P2

// SHARED MEMORY
int shmkey;                         // key for creation of shared memory segment
int shmid;                          // Shared memory ID
struct shmem_seg_struct * shm;      // ptr to shared memory segment struct

//  -   ---    -
// - MAIN DRIVE -
//  -   ---    -
int main(int argc, char ** argv)
{
    build(argv);
    fprintf(stdout, "P:%d:%d: Ready.\n", p_num, getpid());

    char * buffer = (char *) malloc(BUFFSIZE * sizeof(char));
    char * output = (char *) malloc(2 * BUFFSIZE * sizeof(char));

    while(1)
    {
        if(read(pipe_fd[READ], buffer, sizeof(buffer)) > 0)
        {
            //printf("P:%d:%d: Received: %s\n", p_num, getpid(), buffer);

            sem_wait(&shm->mutex_2);

            asciiToHexConvert(buffer, output);
            strcpy(shm->data, output);

            sem_post(&shm->mutex_3);

            memset(buffer, 0, BUFFSIZE);
        }
        
    }

    return 0;
}

// PROCESS INIT
void build(char ** argv)
{
    p_num = atoi(argv[1]);

    pipe_fd[READ]  = atoi(argv[2]);
    pipe_fd[WRITE] = atoi(argv[3]);

    close(pipe_fd[WRITE]);

    // Shared memory creating
    shmkey = ftok("/dev/null", SHMEM_KEY_CODE);
    // Shared memory ID getting
    shmid = shmget (shmkey, sizeof (struct shmem_seg_struct), 0666);
    // Shared memory segment attaching
    shm = (struct shmem_seg_struct *) shmat (shmid, NULL, 0);

    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);
}

// PROCESS DESTROY
void disassemble()
{
    close(pipe_fd[READ]);
}

// SIGNALS
void sigHandler(int signo)
{
    if(signo == SIGINT || signo == SIGUSR1 || signo == SIGUSR2)
    {
        kill(getppid(), signo);
    }
    else if(signo == SIGTERM)
    {
        disassemble();
        signal(SIGTERM, SIG_DFL);
        kill(getpid(), SIGTERM);
    }
}

// FUNCS P2
void asciiToHexConvert(char * input, char * output)
{
    int i = 0;
    char * d2c = input;
    char * n = output;

    while(*d2c != '\0')
    {
        sprintf(n, "%02X ", *d2c++);
        n += 2;
    }
    (*n) = '\0';
}