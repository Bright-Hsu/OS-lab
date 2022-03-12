#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
    pid_t pid,pid1;
    pid=fork();
 
    if (pid<0)
    {
        fprintf(stderr,"fork fail\n");
        return 1;
    }
    else if (pid==0)
    {
        system("ls -al /exp");
        //execlp("/bin/ls","ls","-al","/exp",NULL);
        pid1=getpid();
        printf("child:pid=%d\n",pid);      //A
        printf("child:pid1=%d\n",pid1);     //B
    }
     else
     {
        pid1=getpid();
        printf("parent:pid=%d\n",pid);     //C
        printf("parent:pid1=%d\n",pid1);   //D
        wait(NULL);
     }
    return 0;
}
