#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex12, mutexAB;
pthread_barrier_t barrier;

void threadFunc1(void *arg)
{
	int retval = pthread_mutex_lock(&mutex12);
	if (retval)
	{
		perror("ERROR: pthread_mutex_lock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_lock(...) success!\n");
	}
	int *count = (int*)arg;
	for (int i = 0; i < 100000000; i++)
	{
		(*count)++;
		if ((*count) % 10000000 == 0)
		{
			printf("threadFunc 1: count: %d out of 200000000\n", (*count));
		}
	}
	retval = pthread_mutex_unlock(&mutex12);
	if (retval)
	{
		perror("ERROR: pthread_mutex_unlock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_unlock(...) success!\n");
	}
	return;
}

void threadFunc2(void *arg)
{
	int retval = pthread_mutex_lock(&mutex12);
	if (retval)
	{
		perror("ERROR: pthread_mutex_lock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_lock(...) success!\n");
	}
	int *count = (int*)arg;
	for (int i = 0; i < 100000000; i++)
	{
		(*count)++;
		if ((*count) % 10000000 == 0)
		{
			printf("threadFunc 2: count: %d out of 300000000\n", (*count));
		}
	}
	retval = pthread_mutex_unlock(&mutex12);
	if (retval)
	{
		perror("ERROR: pthread_mutex_unlock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_unlock(...) success!\n");
	}
	return;
}

void threadFunc3(void *arg)
{
	printf("threadFunc3: before wait\n");
	int retval = pthread_barrier_wait(&barrier);
	printf("threadFunc3: after wait, retval: %d\n", retval);
}
void threadFunc4(void *arg)
{
	printf("threadFunc4: before wait\n");
	int retval = pthread_barrier_wait(&barrier);
	printf("threadFunc4: after wait, retval: %d\n", retval);
}

void threadFuncA(void *arg)
{
	int retval = pthread_mutex_lock(&mutexAB);
	if (retval)
	{
		perror("ERROR: pthread_mutex_lock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_lock(...) success!\n");
	}
	int *count = (int*)arg;
	for (int i = 0; i < 100000000; i++)
	{
		(*count)++;
		if ((*count) % 10000000 == 0)
		{
			printf("threadFunc A: count: %d out of 100000000\n", (*count));
		}
	}
	retval = pthread_mutex_unlock(&mutexAB);
	if (retval)
	{
		perror("ERROR: pthread_mutex_unlock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_unlock(...) success!\n");
	}
	return;
}

void threadFuncB(void *arg)
{
	int retval = pthread_mutex_lock(&mutexAB);
	if (retval)
	{
		perror("ERROR: pthread_mutex_lock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_lock(...) success!\n");
	}
	int *count = (int*)arg;
	for (int i = 0; i < 100000000; i++)
	{
		(*count)++;
		if ((*count) % 10000000 == 0)
		{
			printf("threadFunc B: count: %d out of 200000000\n", (*count));
		}
	}
	retval = pthread_mutex_unlock(&mutexAB);
	if (retval)
	{
		perror("ERROR: pthread_mutex_unlock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_unlock(...) success!\n");
	}
	return;
}

int main()
{
	// Test mutex functions.
	int retval = pthread_mutex_init(&mutex12, NULL);
	if (retval)
	{
		perror("ERROR: pthread_mutex_init(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_init(...) success!\n");
	}
	retval = pthread_mutex_init(&mutexAB, NULL);
	if (retval)
	{
		perror("ERROR: pthread_mutex_init(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_init(...) success!\n");
	}
	int count = 0;
	int count_ab = 0;
	pthread_t t1, t2, ta, tb;
	pthread_create(&t1, NULL, (void*)&threadFunc1, (void*)&count);
	pthread_create(&t2, NULL, (void*)&threadFunc2, (void*)&count);
	pthread_create(&ta, NULL, (void*)&threadFuncA, (void*)&count_ab);
	pthread_create(&tb, NULL, (void*)&threadFuncB, (void*)&count_ab);
	retval = pthread_mutex_lock(&mutex12);
	if (retval)
	{
		perror("ERROR: pthread_mutex_lock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_lock(...) success!\n");
	}
	for (int i = 0; i < 100000000; i++)
	{
		count++;
		if (count % 10000000 == 0)
		{
			printf("main: count: %d out of 100000000\n", count);
		}
	}
	retval = pthread_mutex_unlock(&mutex12);
	if (retval)
	{
		perror("ERROR: pthread_mutex_unlock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_unlock(...) success!\n");
	}
	retval = pthread_barrier_init(&barrier, NULL, 3);
	if (retval)
	{
		perror("ERROR: pthread_barrier_init(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_barrier_init(...) success!\n");
	}
	pthread_t t3, t4, t5;
	pthread_create(&t3, NULL, (void*)&threadFunc3, NULL);
	pthread_create(&t4, NULL, (void*)&threadFunc4, NULL);
	printf("main: before wait\n");
	retval = pthread_barrier_wait(&barrier);
	printf("main: after wait, retval: %d\n", retval);
	retval = pthread_barrier_destroy(&barrier);
	if (retval)
	{
		perror("ERROR: pthread_barrier_destroy(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_barrier_destroy(...) success!\n");
	}
	retval = pthread_barrier_init(&barrier, NULL, 3);
	if (retval)
	{
		perror("ERROR: pthread_barrier_init(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_barrier_init(...) success!\n");
	}
	pthread_create(&t3, NULL, (void*)&threadFunc3, NULL);
	pthread_create(&t4, NULL, (void*)&threadFunc4, NULL);
	printf("main: before wait\n");
	retval = pthread_barrier_wait(&barrier);
	printf("main: after wait, retval: %d\n", retval);
	retval = pthread_barrier_destroy(&barrier);
	if (retval)
	{
		perror("ERROR: pthread_barrier_destroy(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_barrier_destroy(...) success!\n");
	}
	pthread_create(&t1, NULL, (void*)&threadFunc1, (void*)&count);
	pthread_create(&t2, NULL, (void*)&threadFunc2, (void*)&count);
	retval = pthread_mutex_lock(&mutex12);
	if (retval)
	{
		perror("ERROR: pthread_mutex_lock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_lock(...) success!\n");
	}
	for (int i = 0; i < 100000000; i++)
	{
		count++;
		if (count % 10000000 == 0)
		{
			printf("main: count: %d out of 100000000\n", count);
		}
	}
	retval = pthread_mutex_unlock(&mutex12);
	if (retval)
	{
		perror("ERROR: pthread_mutex_unlock(...) failed!\n");
		exit(-1);
	}
	else
	{
		printf("pthread_mutex_unlock(...) success!\n");
	}
	pthread_exit(0);
	return 0;
}
