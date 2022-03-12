#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
 
#define MAXSIZE 10
 
int stack[MAXSIZE];
sem_t sem;
 
//provider
void provide_data(void) {
    int i;
    for (i=1; i< MAXSIZE; i++) {
        stack[i] = i;
        printf("stack[i] = %d\n",stack[i]);
        sem_post(&sem);
        sleep(1);
    }
}
 
//consumer
void handle_data(void) {
    int i;
    for (i=1; i< MAXSIZE; i++)
        sem_wait(&sem);
        printf("Multiply: %d X %d = %d\n", stack[i], stack[i], stack[i]*stack[i]);
    }
}
 
int main(void) {
 
    pthread_t provider, handler;
 
    sem_init(&sem, 0, 0);
    pthread_create(&provider, NULL,(void *)handle_data, NULL);
    pthread_create(&handler, NULL,(void *)provide_data, NULL);
    pthread_join(provider, NULL);
    pthread_join(handler, NULL);
    sem_destroy(&sem);
 
    return 0;
}
