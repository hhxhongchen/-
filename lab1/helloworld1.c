#include <stdio.h>
#include <unistd.h>
void main()
{
	char *f1="helloworld!\n";
	int len =11;
	int result=0;
	asm volatile(
	"movl %2, %%edx\n\r"
	"movl %1, %%ecx\n\r"
	"movl $1, %%ebx\n\r"
	"movl $4, %%eax\n\r"
	"int $0x80"
	:"=m"(result)
	:"m"(f1),"r"(len)
	:"%eax");
}
