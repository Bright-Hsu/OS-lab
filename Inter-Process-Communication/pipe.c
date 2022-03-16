#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
int pid1,pid2;
int main(){
    int fd[2];
    char InPipe[1000];
    char c1='1',c2='2';
    pipe(fd);
    while((pid1=fork())==-1);
    if(pid1==0){
        //锁定管道
        //分200次，每次向管道写入字符'1'
        lockf(fd[1],1,0);
        int i=0;
        for (i=0;i<200;i++){
            write(fd[1],&c1,1);
        }
        sleep(5);
        //解除管道锁定
        lockf(fd[1],0,0);
        exit(0);
    }
    else{
        while((pid2=fork())==-1);
        if(pid2==0){
            lockf(fd[1],1,0);
            //分200次每次向管道写入字符'2'
            int i=0;
            for (i=0;i<200;i++){
                write(fd[1],&c2,1);
            }
            sleep(5);
            lockf(fd[1],0,0);
            exit(0);
        }
        else{
            wait(0);
            wait(0);
            //从管道读出400个字符
            //while(read(fd[0],InPipe,1)>0);
            read(fd[0],InPipe,400);
            InPipe[400]='\0';
            printf("%s\n",InPipe);
            exit(0);
        }
    }
}
