#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex;
pthread_barrier_t barrier;

void threadFunc1(void *arg)
{
	int *count = (int*)arg;
	int retval = pthread_mutex_lock(&mutex);
	if (retval)
	{
		perror("lock fail in threadFunc1!\n");
		exit(-1);
	}
	for (int i = 0; i < 100000000; i++)
	{
		(*count)++;
		if (!((*count) % 10000000))
		{
			printf("threadFunc1 progress: %d%%\n", i / 2000000);
		}
	}
	retval = pthread_mutex_unlock(&mutex);
	if (retval)
	{
		perror("unlock fail in threadFunc1!\n");
		exit(-1);
	}
	retval = pthread_barrier_wait(&barrier);
	if (retval)
	{
		printf("PTHREAD_BARRIER_SERIAL_THREAD caught in threadFunc1.\n");
	}
	retval = pthread_mutex_lock(&mutex);
	if (retval)
	{
		perror("second time lock fail in threadFunc1!\n");
		exit(-1);
	}
	for (int i = 0; i < 100000000; i++)
	{
		(*count)++;
		if (!((*count) % 10000000))
		{
			printf("threadFunc1 progress: %d%%\n", i / 2000000 + 50);
		}
	}
	retval = pthread_mutex_unlock(&mutex);
	if (retval)
	{
		perror("second time unlock fail in threadFunc1!\n");
		exit(-1);
	}
}

void threadFunc2(void *arg)
{
	int *count = (int*)arg;
	int retval = pthread_mutex_lock(&mutex);
	if (retval)
	{
		perror("lock fail in threadFunc2!\n");
		exit(-1);
	}
	for (int i = 0; i < 100000000; i++)
	{
		(*count)++;
		if (!((*count) % 10000000))
		{
			printf("threadFunc2 progress: %d%%\n", i / 2000000);
		}
	}
	retval = pthread_mutex_unlock(&mutex);
	if (retval)
	{
		perror("unlock fail in threadFunc2!\n");
		exit(-1);
	}
	retval = pthread_barrier_wait(&barrier);
	if (retval)
	{
		printf("PTHREAD_BARRIER_SERIAL_THREAD caught in threadFunc2.\n");
	}
	retval = pthread_mutex_lock(&mutex);
	if (retval)
	{
		perror("second time lock fail in threadFunc2!\n");
		exit(-1);
	}
	for (int i = 0; i < 100000000; i++)
	{
		(*count)++;
		if (!((*count) % 10000000))
		{
			printf("threadFunc2 progress: %d%%\n", i / 2000000 + 50);
		}
	}
	retval = pthread_mutex_unlock(&mutex);
	if (retval)
	{
		perror("second time unlock fail in threadFunc2!\n");
		exit(-1);
	}
}

int main()
{
	int retval = pthread_mutex_init(&mutex, NULL);
	if (retval)
	{
		perror("mutex init fail in main!\n");
		exit(-1);
	}
	retval = pthread_barrier_init(&barrier, NULL, 3);
	if (retval)
	{
		perror("barrier init fail in main!\n");
		exit(-1);
	}
	pthread_t t1, t2;
	int count = 0;
	pthread_create(&t1, NULL, (void*)&threadFunc1, (void*)&count);
	pthread_create(&t2, NULL, (void*)&threadFunc2, (void*)&count);
	retval = pthread_mutex_lock(&mutex);
	if (retval)
	{
		perror("lock fail in main!\n");
		exit(-1);
	}
	for (int i = 0; i < 100000000; i++)
	{
		count++;
		if (!(count % 10000000))
		{
			printf("main progress: %d%%\n", i / 2000000);
		}
	}
	retval = pthread_mutex_unlock(&mutex);
	if (retval)
	{
		perror("unlock fail in main!\n");
		exit(-1);
	}
	retval = pthread_barrier_wait(&barrier);
	if (retval)
	{
		printf("PTHREAD_BARRIER_SERIAL_THREAD caught in threadFunc2.\n");
	}
	retval = pthread_mutex_lock(&mutex);
	if (retval)
	{
		perror("second time lock fail in main!\n");
		exit(-1);
	}
	for (int i = 0; i < 100000000; i++)
	{
		count++;
		if (!(count % 10000000))
		{
			printf("main progress: %d%%\n", i / 2000000 + 50);
		}
	}
	retval = pthread_mutex_unlock(&mutex);
	if (retval)
	{
		perror("second time unlock fail in main!\n");
		exit(-1);
	}
	pthread_exit(0);
	return 0;
}
