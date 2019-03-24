#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	pid_t pid;
	pid=fork();
	if(pid>0)
	 while(1);
	else if(!pid)
	{
		int ret;
		ret=execlp("vi","",NULL);
		if(ret==-1)
		 perror("execl");
	}
	else if(pid==-1)
	 perror("fork");
	return 0;
}
