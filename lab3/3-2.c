#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

sem_t *empty=NULL;//记录剩余空间
sem_t *full=NULL;//记录剩余票数
sem_t *mutex=NULL;
int temp;
int sell=30;
int return0=7;
int ticketCount=1000;
int pthread_yield(void);
int i=0;
int j=0;
//售票
void * consumer(){
    for(;i<sell;i++){
        sem_wait(full);
        sem_wait(mutex);
        temp=ticketCount;
        pthread_yield();
        temp=temp-1;
        pthread_yield();
        ticketCount=temp;
        printf("[售票 - %d]\n",i+1);
        sem_post(empty);
        sem_post(mutex);
    }

}
//退票
void * producer(){
    for(;j<return0;j++)
    {
	sem_wait(empty);
	sem_wait(mutex);
	temp=ticketCount;
	pthread_yield();
	temp=temp+1;
	pthread_yield();
	ticketCount=temp;
        printf("{退票 + %d}\n",j+1);
        sem_post(full);
        sem_post(mutex);
    }

}

int main()
{
	pthread_t p1,p2;
	full=sem_open("full",O_CREAT,0666,1000);//记录剩余票数
	empty=sem_open("empty",O_CREAT,0666,0);//记录剩余空间
	mutex=sem_open("mutex",O_CREAT,0666,1);
	printf("[Begin]:剩余票数为 :%d\n",ticketCount);
	pthread_create(&p1,NULL,consumer,NULL);
	pthread_create(&p2,NULL,producer,NULL);
	pthread_join(p2,NULL);
	pthread_join(p1,NULL);
	printf("[总共售票:%d]\n",i);
	printf("{总共退票:%d}\n",j);
	printf("[End]:剩余票数为 :%d\n",ticketCount);
	sem_close(full);
	sem_close(empty);
	sem_close(mutex);
	sem_unlink("empty");
	sem_unlink("mutex");
	sem_unlink("full");
	return 0;
}

