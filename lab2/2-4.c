#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
 pid_t p1,p2,p3,p4,p5;
 int i=0;
 while ((p1=fork())==-1);
 if (!p1) 
 {
	while ((p2=fork())==-1);
	if (!p2) 
	{
	  while((p4=fork())==-1);
          if(!p4) 
	  {
	    while(1)
	    {
      	      printf("p4 pid %d, parent pid %d.\n",getpid(),getppid());
	      sleep(2);
	    }
	  }
      	  else 
	  {
      	    while((p5=fork())==-1);
      	    if(!p5) 
	    {
	      while(1)
	      {
      	        printf("p5 pid %d, parent pid %d.\n",getpid(),getppid());
		sleep(2);
	      }
	    }
      	  }
	  while(1)
	  {
	    printf("p2 pid %d, parent pid %d.\n",getpid(),getppid());
            
	    /*i++;
	    if(i==2)
	    {
		int *p=NULL;
                *p=0;
	    }*/ //段地址错误
            /*i++;
            if(i==2)
	        exit(0);*/ //exit（）函数
	    sleep(2);
	  }
    	}
    	else 
	{
    	  while ((p3=fork())==-1);
    	  if (!p3) 
   	  {
	    while(1)
	    {
      	      printf("p3 pid %d, parent pid %d.\n",getpid(),getppid());
	      sleep(2);
	    }
	  }
 	}
	while(1)
	{
      	   printf("p1 pid %d, parent pid %d.\n",getpid(),getppid());
           sleep(2);
	}

  }
  return 0;
}
