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

int p_num;  // Process number

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
    int i = 0;
    char c = 0;
    int chr_printed = 0;
    int pos = 0;

    while(1)
    {
        sem_wait(&shm->mutex_3);
        
        strcpy(buffer, shm->data);

        c = buffer[i];
        
        while(c != '\0')
        {
            printf("%c", c);
            chr_printed++;
            
            if(chr_printed == 2){
                printf(" ");
                pos++;
                chr_printed = 0;
            }

            if(pos == 15){
                printf("\n");
                pos = 0;
            }

            i++;
            c = buffer[i];
        }

        i = 0;

        memset(buffer, 0, BUFFSIZE);

        sem_post(&shm->mutex_2);
    }

    return 0;
}

// PROCESS INIT
void build(char ** argv)
{
    p_num = atoi(argv[1]);

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
    ;;
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