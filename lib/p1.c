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
void disassemble();
void sigHandler(int signo);

void menu();
void manual();
void file();

int running;

int p_num;          // Process number

// PIPE
int pipe_fd[2];     // Pipe: P1 -> P2

//  -   ---    -
// - MAIN DRIVE -
//  -   ---    -
int main(int argc, char ** argv)
{
    build(argv);
    fprintf(stdout, "P:%d:%d: Ready.\n", p_num, getpid());

    while(1)
    {
        menu();
    }

    return 0;
}

// PROCESS INIT
void build(char ** argv)
{
    running = 1;
    p_num = atoi(argv[1]);

    pipe_fd[READ]  = atoi(argv[2]);
    pipe_fd[WRITE] = atoi(argv[3]);

    close(pipe_fd[READ]);

    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);
}

// PROCESS DESTROY
void disassemble()
{
    close(pipe_fd[WRITE]);
}

// SIGNALS
void sigHandler(int signo)
{
    if(signo == SIGINT)
    {
        kill(getppid(), signo);
    }
    else if(signo == SIGUSR1)
    {
        running = 1;
    }
    else if(signo == SIGUSR2)
    {
        running = 1;
    }
    else if(signo == SIGTERM)
    {
        disassemble();
        signal(SIGTERM, SIG_DFL);
        kill(getpid(), SIGTERM);
    }
}

// FUNCS P1
void menu()
{
    char opt;

    usleep(500);

    printf("\n\n");
    printf("[1] - Manual\n");
    printf("[2] - File\n");
    printf("Choose option:\n");
    printf(" > ");
    scanf("%c", &opt);

    if(opt == '1'){
        manual();
    }
    else if(opt == '2'){
        file();
    }
    else{
        printf("Choose valid option!\n\n");
    }
}

void manual()
{
    char * buffer = (char *) malloc(BUFFSIZE * sizeof(char));

    while(1)
    {
        printf("\nType the data ('exit' to back to the menu):\n");
        printf(" > ");
        fgets(buffer, sizeof(buffer), stdin);

        if(strncmp(buffer, "exit\n", 5) == 0)
        {
            free(buffer);
            break;
        }

        write(pipe_fd[WRITE], buffer, strlen(buffer));
        memset(buffer, 0, BUFFSIZE);
    }
}

void file()
{
    char filepath[BUFFSIZE];

    printf("\nType the filepath:\n");
    printf(" > ");
    scanf("%s", filepath);

    FILE * fp = fopen(filepath, "r");
    if(fp == NULL)
    {
        printf("\n%s : Cannot open this file!\n\n", filepath);
    }
    else
    {
        char * buffer = (char *) malloc(BUFFSIZE * sizeof(char));

        while(1)
        {
            if(fgets(buffer, sizeof(buffer), fp) == NULL) break;
            write(pipe_fd[WRITE], buffer, strlen(buffer));
            memset(buffer, 0, BUFFSIZE);
        }

        free(buffer);
        fclose(fp);
    }
}