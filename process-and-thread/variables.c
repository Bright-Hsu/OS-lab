#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int count=1;
int main()
{
    pid_t pid,pid1;
    pid=fork();

    if (pid<0)
    {
        fprintf(stderr,"fork fail");
        return 1;
    }
    else if (pid==0)
    {
        pid1=getpid();
        count++;
        printf("In the process of the child, count=%d\n",count);
        printf("child:pid=%d\n",pid);      //A
        printf("child:pid1=%d\n",pid1);     //B
    }
    else
    {
        pid1=getpid();
        count*=10;
        printf("In the process of the parent, count=%d\n",count);
        printf("parent:pid=%d\n",pid);     //C
        printf("parent:pid1=%d\n",pid1);   //D
        wait(NULL);
}

    count+=5;
    printf("In the main function, count=%d\n",count);
    return 0;
}