#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
 
void function1 (void *ptr);
void function2 (void *ptr);
int num=5; 

int main()
{
    int tmp1, tmp2;
    pthread_t thread1, thread2;
    char *message1 = "thread1";
    char *message2 = "thread2";
 
    int ret_thrd1, ret_thrd2;
 
    ret_thrd1 = pthread_create(&thread1, NULL, (void *)&function1, (void *) message1);
    ret_thrd2 = pthread_create(&thread2, NULL, (void *)&function2, (void *) message2);
 
    if (ret_thrd1 != 0) {
        printf("thread1 creation is failed.\n");
} 
else {
        printf("thread1 creation succeeded.\n");
    }
 
    if (ret_thrd2 != 0) {
        printf("thread2 creation is failed.\n");
} 
else {
        printf("thread2 creation succeeded.\n");
    }

    tmp1 = pthread_join(thread1, NULL);
    if (tmp1 != 0) {
        printf("cannot join with thread1\n");
    }
    printf("thread1 end\n");
 
    tmp2 = pthread_join(thread2, NULL);
    if (tmp2 != 0) {
        printf("cannot join with thread2\n");
    }
    printf("thread2 end\n");
printf("Multithreading is over!\n");
    printf("num=%d\n", num);
    return 0;
}
 
void function1( void *ptr ) {
    int i = 1;
    for (i; i<num; i++) {
        printf("%s:%d+%d=%d\n", (char *)ptr, i, i, i+i);
    }
    num=7;
}
void function2( void *ptr ) {
    int i = 1;
    for (i; i<num; i++) {
        printf("%s:%d*%d=%d\n", (char *)ptr, i, i, i*i);
    }
    num=9;
}
