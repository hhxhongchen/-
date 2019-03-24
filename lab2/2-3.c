#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void) {
 int p1,p2,p3,p4,p5;
 while ((p1=fork())==-1);
 if (!p1) {
    printf("p1 pid %d, ppid %d.\n",getpid(),getppid());
    while ((p2=fork())==-1);
    if (!p2) {
      printf("p2 pid %d, ppid %d.\n",getpid(),getppid());
      while((p4=fork())==-1);
      if(!p4){
	printf("p4 pid %d, ppid %d.\n",getpid(),getppid());
      	exit(0);
      }
      else 
	wait(0);
      while((p5=fork())==-1);
      if(!p5){
	printf("p5 pid %d, ppid %d.\n",getpid(),getppid());
      	exit(0);
      }
      else 
	wait(0); 
      exit(0);
    }
    else
      wait(0);
    while ((p3=fork())==-1);
    if (!p3) {
      printf("p3 pid %d, ppid %d.\n",getpid(),getppid());
      exit(0);
    }
    else
      wait(0);
    exit(0);
 }
  else
    wait(0);
  return 0;
}
