#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

sem_t* sem1 = NULL;
sem_t* sem2 = NULL;
sem_t* sem4 = NULL;

int main(){
    pid_t p1,p2,p3;
    sem1 = sem_open("1", O_CREAT, 0666, 0);
    sem4 = sem_open("4", O_CREAT, 0666, 0);
    sem2 = sem_open("2", O_CREAT, 0666, 0);

    p1 = fork();
    if (p1>0){
    	p2 = fork();
        if(p2>0){
                sem_wait(sem1);
                sem_wait(sem4);
                printf("I am the process 4\n");
        }
        else{
                printf("I am the process 1\n");
                sem_post(sem2);
        }
    }
    else if (p1==0){
	p3 = fork();
        if(p3>0){
            sem_wait(sem2);
            printf("I am the process 2\n");
            sem_post(sem2);
            sem_post(sem4);
        }
        else{
            sem_wait(sem2);
            printf("I am the process 3\n");
            sem_post(sem2);
            sem_post(sem1);
        }
    }

    return 0;
}
