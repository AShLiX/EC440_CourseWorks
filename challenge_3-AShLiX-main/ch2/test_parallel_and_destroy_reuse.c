#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

pthread_barrier_t barrier12, barrierAB;

void loopThread1(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThread1 progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier12);
    }
}

void loopThread2(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThread2 progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier12);
    }
}

void loopThreadA(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadA progress: %d\n", i);
        retval = pthread_barrier_wait(&barrierAB);
    }
}

void loopThreadB(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadB progress: %d\n", i);
        retval = pthread_barrier_wait(&barrierAB);
    }
}

void loopThreadX(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadX progress: %d\n", i);
        retval = pthread_barrier_wait(&barrierAB);
    }
}

void loopThreadY(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadY progress: %d\n", i);
        retval = pthread_barrier_wait(&barrierAB);
    }
    pthread_barrier_destroy(&barrierAB);
}

int main()
{
    int retval = pthread_barrier_init(&barrier12, NULL, 3);
    if (retval)
    {
        perror("barrier12 init fail!\n");
        exit(-1);
    }
    retval = pthread_barrier_init(&barrierAB, NULL, 2);
    if (retval)
    {
        perror("barrierAB init fail!\n");
        exit(-1);
    }
    pthread_t t1, t2, tA, tB, tX, tY;
    pthread_create(&t1, NULL, (void*)&loopThread1, NULL);
    pthread_create(&t2, NULL, (void*)&loopThread2, NULL);
    pthread_create(&tA, NULL, (void*)&loopThreadA, NULL);
    pthread_create(&tB, NULL, (void*)&loopThreadB, NULL);
    for (int i = 0; i < 10; i++)
    {
        printf("main progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier12);
    }
    retval = pthread_barrier_destroy(&barrierAB);
    if (retval)
    {
        perror("barrier destroy fail in main!\n");
    }
    retval = pthread_barrier_init(&barrierAB, NULL, 2);
    if (retval)
    {
        perror("second time barrierAB init fail!\n");
    }
    pthread_create(&tX, NULL, (void*)&loopThreadX, NULL);
    pthread_create(&tY, NULL, (void*)&loopThreadY, NULL);
    pthread_exit(0);
    return 0;
}