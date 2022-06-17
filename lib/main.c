// LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>

//HEADERS
#include "consts.h"

// FUNCTIONS
int buildTheSyncStructures();
int removeTheSyncStructures();
int callTheChildProcesses();
void sigHandler(int signo);

// SHARED MEMORY
int shmkey;                         // key for creation of shared memory segment
int shmid;                          // Shared memory ID
struct shmem_seg_struct * shm;      // ptr to shared memory segment struct

// PIPES
int pipe_1_2[2];                    // pipe: P1 -> P2

// CHILD PROCESSES
pid_t p1, p2, p3;                   // Child processes IDs

//  -   ---    -
// - MAIN DRIVE -
//  -   ---    -
int main()
{
    int fork_passed = 1;

    fprintf(stdout, "M: Operaion: Building the synchronization structures...\n");
    if(buildTheSyncStructures() != 0){
        fprintf(stdout, "M: Operation: Building the synchronization structures!\n");
        fprintf(stdout, "\nM: Result: Program has stoped with FAILURE!\n");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stdout, "M: Operation result: The synchronization structures build successfully.\n\n");
    }

    fprintf(stdout, "M: Operation: Calling the child processes...\n");
    if(callTheChildProcesses() == 1){
        fprintf(stdout, "M: Operation result: Forking the child processes has failed\n\n");
        fork_passed = 0;
    } else {
        fprintf(stdout, "M: Operation result: Child processes are successfully done!\n\n");
    }

    fprintf(stdout, "M: Operation: Removing the synchronization structures...\n");
    if(removeTheSyncStructures() != 0){
        fprintf(stdout, "M: Operation: Removing the synchronization structures!\n\n");
        fprintf(stdout, "\nM: Result: Program has stoped with FAILURE!\n");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stdout, "M: Operation result: The synchronization structures removed successfully.\n");
    }

    if(fork_passed){
        fprintf(stdout, "\nM: Result: Program has stoped sith SUCCESS!\n");
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stdout, "\nM: Result: Program has stoped with FAILURE!\n");
        exit(EXIT_FAILURE);
    }
}

// SYNC STRUCTURES
int buildTheSyncStructures()
{
    // Pipes creating
    if(pipe(pipe_1_2) == -1){
        perror("M: Error: pipe");
        return 1;
    } else {
        fprintf(stdout, "M: Build: Pipe.\n");
    }

    // Shared memory creating
    if((shmkey = ftok("/dev/null", SHMEM_KEY_CODE)) == -1){
        perror("M: Error: ftok");
        return 1;
    } else {
        fprintf(stdout, "M: Build: Shared memory key.\n");
    }

    // Shared memory ID getting
    if ((shmid = shmget (shmkey, sizeof (struct shmem_seg_struct), 0666 | IPC_CREAT | IPC_EXCL)) < 0){
        perror("M: Error: shmget");
        return 1;
    } else {
        fprintf(stdout, "M: Build: Shared memory segment.\n");
    }

    // Shared memory segment attaching
    if((shm = (struct shmem_seg_struct *) shmat (shmid, NULL, 0)) == (struct shmem_seg_struct *)-1){
        perror("M: Error: shmat");
        return 1;
    } else {
        fprintf(stdout, "M: Build: Shared memory segment attached.\n");
    }

    // Semaphore init: mutex_2
    if(sem_init(&shm->mutex_2, 1, 1) != 0){
        perror("M: Error: sem_init(mutex_2)");
        return 1;
    } else {
        fprintf(stdout, "M: Build: Semaphore initialized: mutex_2.\n");
    }

    // Semaphore init: mutex_3
    if(sem_init(&shm->mutex_3, 1, 0) != 0){
        perror("M: Error: sem_init(mutex_3)");
        return 1;
    } else {
        fprintf(stdout, "M: Build: Semaphore initialized: mutex_3.\n");
    }

    return 0;
}

int removeTheSyncStructures()
{
    // Pipe READ closing
    if(close(pipe_1_2[READ]) == -1){
        perror("M: close(pipe[READ])");
        return 1;
    } else {
        fprintf(stdout, "M: Removed: Pipe [READ] closed.\n");
    }

    // Pipe WRITE closing
    if(close(pipe_1_2[WRITE]) == -1){
        perror("M: close(pipe[WRITE])");
        return 1;
    } else {
        fprintf(stdout, "M: Removed: Pipe [WRITE] closed.\n");
    }

    // Semaphore destroy: mutex_2
    if(sem_destroy(&shm->mutex_2) != 0){
        perror("M: Error: sem_destroy(mutex_2)");
        return 1;
    } else {
        fprintf(stdout, "M: Removed: Semaphore destroyed: mutex_2.\n");
    }

    // Semaphore destroy: mutex_3
    if(sem_destroy(&shm->mutex_3) != 0){
        perror("M: Error: sem_destroy(mutex_3)");
        return 1;
    } else {
        fprintf(stdout, "M: Removed: Semaphore destroyed: mutex_3.\n");
    }
    
    // Shared memory detaching
    if(shmdt (shm) != 0){
        perror("M: Error: shmdt");
        return 1;
    } else {
        fprintf(stdout, "M: Removed: Shared memory segment detached.\n");
    }

    // Shared memory removing
    if(shmctl (shmid, IPC_RMID, 0) == -1){
        perror("M: Error: shmctl");
        return 1;
    } else {
        fprintf(stdout, "M: Removed: Shared memory segment removed.\n");
    }

    return 0;
}

// CHILD PROCESSES
int callTheChildProcesses()
{
    // Preparing data to pass
    char * pipe_r = (char *) malloc(16 * sizeof(char));
    char * pipe_w = (char *) malloc(16 * sizeof(char));

    sprintf(pipe_r, "%d", pipe_1_2[READ]);
    sprintf(pipe_w, "%d", pipe_1_2[WRITE]);

    // Forking the child processes
    ((p1 = fork()) && (p2 = fork()) && (p3 = fork()));
    
    // CHILD PROCES 1
    if(p1 == 0){
        char* args[] = {p1_src_file, "1", pipe_r, pipe_w, NULL};
        execvp(p1_src_file, args);
        exit(EXIT_SUCCESS);
    }

    // CHILD PROCES 2
    else if(p2 == 0){
        char* args[] = {p2_src_file, "2", pipe_r, pipe_w, NULL};
        execvp(p2_src_file, args);
        exit(EXIT_SUCCESS);
    }

    // CHILD PROCES 3
    else if(p3 == 0){
        char* args[] = {p3_src_file, "3", NULL};
        execvp(p3_src_file, args);
        exit(EXIT_SUCCESS);
    }

    // PARENT PROCES MAIN
    else{
        signal(SIGINT,  sigHandler);
        signal(SIGUSR1, sigHandler);
        signal(SIGUSR2, sigHandler);

        // Waiting for child processes to end
        wait(NULL);
        wait(NULL);
        wait(NULL);
    }
}

// SIGNALS
void sigHandler(int signo)
{
    if(signo == SIGINT)
    {
        kill(p1, SIGTERM);
        kill(p2, SIGTERM);
        kill(p3, SIGTERM);
    }
    else if(signo == SIGUSR1)
    {
        kill(p1, SIGUSR1);
    }
    else if(signo == SIGUSR2)
    {
        kill(p1, SIGUSR2);
    }
}