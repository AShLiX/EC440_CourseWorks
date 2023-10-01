#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

pthread_barrier_t barrier, BARRIER;

void loopThread1(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThread1 progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in t1\n");
            exit(-1);
        }
    }
}

void loopThread2(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThread2 progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in t2\n");
            exit(-1);
        }
    }
}

void loopThreadA(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadA progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in tA\n");
            exit(-1);
        }
    }
}

void loopThreadB(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadB progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in tB\n");
            exit(-1);
        }
    }
}

void loopThreadC(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadC progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in tC\n");
            exit(-1);
        }
    }
}

void loopThreadX(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadX progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in tX\n");
            exit(-1);
        }
    }
}

void loopThreadY(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadY progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in tY\n");
            exit(-1);
        }
    }
}

void loopThread1u(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThread1u progress: %d\n", i);
        retval = pthread_barrier_wait(&BARRIER);
        if (retval)
        {
            perror("BARRIER wait fail in t1\n");
            exit(-1);
        }
    }
}

void loopThread2u(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThread2u progress: %d\n", i);
        retval = pthread_barrier_wait(&BARRIER);
        if (retval)
        {
            perror("BARRIER wait fail in t2\n");
            exit(-1);
        }
    }
}

void loopThreadAu(void *arg)
{
    int retval;
    for (int i = 0; i < 10; i++)
    {
        printf("loopThreadAu progress: %d\n", i);
        retval = pthread_barrier_wait(&BARRIER);
        if (retval)
        {
            perror("BARRIER wait fail in tA\n");
            exit(-1);
        }
    }
}

int main()
{
    pthread_t t1, t2, tA, tB, tC, tX, tY;
    pthread_t t1u, t2u, tAu;
    int retval = pthread_barrier_init(&barrier, NULL, 3);
    if (retval)
    {
        perror("First time barrier init fail!\n");
        exit(-1);
    }
    retval = pthread_barrier_init(&BARRIER, NULL, 3);
    if (retval)
    {
        perror("First time BARRIER init fail!\n");
        exit(-1);
    }
    pthread_create(&t1, NULL, (void*)&loopThread1, NULL);
    pthread_create(&t2, NULL, (void*)&loopThread2, NULL);
    pthread_create(&t1u, NULL, (void*)&loopThread1u, NULL);
    pthread_create(&t2u, NULL, (void*)&loopThread2u, NULL);
    for (int i = 0; i < 10; i++)
    {
        printf("main progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in main\n");
            exit(-1);
        }
    }
    for (int i = 0; i < 10; i++)
    {
        printf("main progress: %d\n", i);
        retval = pthread_barrier_wait(&BARRIER);
        if (retval)
        {
            perror("BARRIER wait fail in main\n");
            exit(-1);
        }
    }
    retval = pthread_barrier_destroy(&barrier);
    if (retval)
    {
        perror("First time barrier destroy fail!\n");
    }
    else
    {
        printf("-------------barrier DESTROY-------------\n");
    }
    retval = pthread_barrier_destroy(&BARRIER);
    if (retval)
    {
        perror("First time BARRIER destroy fail!\n");
    }
    else
    {
        printf("-------------BARRIER DESTROY-------------\n");
    }
    retval = pthread_barrier_wait(&barrier);
    if (!retval)
    {
        printf("Why can I still use barrier after destroying it?\n");
    }
    retval = pthread_barrier_wait(&BARRIER);
    if (!retval)
    {
        printf("Why can I still use BARRIER after destroying it?\n");
    }
    retval = pthread_barrier_init(&barrier, NULL, 4);
    if (retval)
    {
        perror("Second time barrier init fail!\n");
    }
    retval = pthread_barrier_init(&BARRIER, NULL, 2);
    if (retval)
    {
        perror("Second time BARRIER init fail!\n");
    }
    pthread_create(&tA, NULL, (void*)&loopThreadA, NULL);
    pthread_create(&tB, NULL, (void*)&loopThreadB, NULL);
    pthread_create(&tC, NULL, (void*)&loopThreadC, NULL);
    pthread_create(&tAu, NULL, (void*)&loopThreadAu, NULL);
    for (int i = 0; i < 10; i++)
    {
        printf("main progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in main\n");
            exit(-1);
        }
    }
    for (int i = 0; i < 10; i++)
    {
        printf("main progress: %d\n", i);
        retval = pthread_barrier_wait(&BARRIER);
        if (retval)
        {
            perror("BARRIER wait fail in main\n");
            exit(-1);
        }
    }
    retval = pthread_barrier_destroy(&barrier);
    if (retval)
    {
        perror("Second time barrier destroy fail!\n");
    }
    else
    {
        printf("-------------barrier DESTROY-------------\n");
    }
    retval = pthread_barrier_destroy(&BARRIER);
    if (retval)
    {
        perror("Second time BARRIER destroy fail!\n");
    }
    else
    {
        printf("-------------BARRIER DESTROY-------------\n");
    }
    retval = pthread_barrier_wait(&barrier);
    if (!retval)
    {
        printf("Why can I still use barrier after destroying it?\n");
    }
    retval = pthread_barrier_wait(&BARRIER);
    if (!retval)
    {
        printf("Why can I still use BARRIER after destroying it?\n");
    }
    retval = pthread_barrier_init(&barrier, NULL, 3);
    if (retval)
    {
        perror("Third time barrier init fail!\n");
    }
    pthread_create(&tX, NULL, (void*)&loopThreadX, NULL);
    pthread_create(&tY, NULL, (void*)&loopThreadY, NULL);
    for (int i = 0; i < 10; i++)
    {
        printf("main progress: %d\n", i);
        retval = pthread_barrier_wait(&barrier);
        if (retval)
        {
            perror("barrier wait fail in main\n");
            exit(-1);
        }
    }
    retval = pthread_barrier_destroy(&barrier);
    if (retval)
    {
        perror("Third time barrier destroy fail!\n");
    }
    else
    {
        printf("-------------DESTROY-------------\n");
    }
    pthread_exit(0);
    return 0;
}
