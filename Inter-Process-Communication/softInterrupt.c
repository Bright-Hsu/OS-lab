#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
int wait_flag=0;
void stop(int signum);

int main(){
    int pid1,pid2;
    //signal(SIGINT,stop);
    signal(SIGQUIT,stop);
    while((pid1=fork())==-1);
    if(pid1>0){
        while((pid2=fork())==-1);
        if(pid2>0){   //父进程
            wait_flag=1;
            //父进程等待5秒
            sleep(5);
            //杀死进程1发中断号16
            kill(pid1,SIGUSR1);
            kill(pid2,SIGUSR2);
            wait(0);
            wait(0);
            printf("\n Parent process is killed !!\n");
            exit(0);
        }
        else{    //子进程2
            wait_flag=1;
            signal(SIGUSR2,stop);
            //等待进程2被杀死的中断号17
            while(wait_flag==1);
            printf("\n Child process 2 is killed by parent !!\n");
            exit(0);
        }
    }
    else{   //子进程1
        wait_flag=1;
        signal(SIGUSR1,stop);
        while(wait_flag==1);
        printf("\n Child process 1 is killed by parent !!\n");
        exit(0);
    }
}

void stop(int signum){
    wait_flag=0;
    printf("\n %d stop test \n",signum);
}
