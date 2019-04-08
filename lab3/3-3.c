#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include<fcntl.h>

#define LOOP_COUNT 4	//生产者和消费者各自循环次数
int a;
int i=0,t=0;
int buf[LOOP_COUNT];
pthread_mutex_t mutex;
sem_t *empty=NULL,*full=NULL;
void *producer( void *arg );	//生产者线程
void *consumer( void *arg );	//消费者线程
int main(int argc , char *argv[]){
	pthread_t thrd_prod , thrd_cons;
	empty = sem_open("empty",O_CREAT,0666,4);
	full = sem_open("full",O_CREAT,0666,0);
	//创建生产者和消费者线程
	pthread_create(&thrd_prod,NULL,producer,NULL);
	pthread_create(&thrd_cons,NULL,consumer,NULL);
	pthread_join(thrd_prod,NULL);
	pthread_join(thrd_cons,NULL);
	sem_close(empty);
	sem_close(full);
	sem_unlink("empty");
	sem_unlink("full");
	return 0;
}
 
void *producer( void *arg){
	int count = 0 ;				//循环计数
	while( count < LOOP_COUNT ){
		sleep(1);
		printf("输入字符:");
		scanf("%d",&a);
		buf[i]=a;
		i++;	
		count++;
	}
}
void *consumer( void *arg ){
	int count = 0 ;//循环计数
	while( count < LOOP_COUNT ){
		sleep(2);
		printf("输出字符:%d\n",buf[t]);
		t++;
		count++;
	}
}

