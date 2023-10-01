#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "tls.h"

pthread_mutex_t mutex;
pthread_barrier_t barrier;

void * threadFuncA(void *arg)
{
    pthread_mutex_lock(&mutex);
    unsigned int size = (unsigned int)10000;
    int retval;
    retval = tls_create(size);
    if (retval)
    {
        perror("threadFuncA tls_create fail.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncA tls_create success.\n");
    }
    char *char_ptr = (char*)calloc(10000, sizeof(char));
    memset(char_ptr, 'a', 10000);
    retval = tls_write(0, size, char_ptr);
    if (retval)
    {
        perror("threadFuncA tls_write fail.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncA tls_write success.\n");
    }
    char *char_ptr2 = (char*)calloc(10000, sizeof(char));
    retval = tls_read(0, size, char_ptr2);
    if (retval)
    {
        perror("threadFuncA tls_read fail.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncA tls_read success.\n");
    }
    retval = strcmp(char_ptr, char_ptr2);
    if (retval)
    {
        perror("threadFuncA read data different from write data.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncA read data same as write data.\n");
    }
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
    pthread_mutex_lock(&mutex);
    retval = tls_read(0, 10000, char_ptr2);
    if (retval)
    {
        perror("second time threadFuncA tls_read fail.\n");
        exit(-1);
    }
    else
    {
        printf("second time threadFuncA tls_read success.\n");
    }
    retval = strcmp(char_ptr, char_ptr2);
    if (retval)
    {
        perror("second time threadFuncA read data different from write data.\n");
        exit(-1);
    }
    else
    {
        printf("second time threadFuncA read data same as write data.\n");
    }
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
    /*
    retval = tls_destroy();
    if (retval)
    {
        perror("threadFuncA tls_destroy fail.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncA tls_destroy success.\n");
    }
    */
    free(char_ptr);
    free(char_ptr2);
    pthread_exit(0);
}

void * threadFuncClone1(void *arg)
{
    pthread_t *tA = (pthread_t*)arg;
    int retval;
    retval = tls_clone(*tA);
    if (retval)
    {
        perror("threadFuncClone1 tls_clone fail.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncClone1 tls_clone success.\n");
    }
    char *char_ptr = (char*)calloc(10000, sizeof(char));
    char *char_ptr2 = (char*)calloc(10000, sizeof(char));
    memset(char_ptr2, 'a', 10000);
    retval = tls_read(0, 10000, char_ptr);
    if (retval)
    {
        perror("threadFuncClone1 tls_read fail.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncClone1 tls_read success.\n");
    }
    retval = strcmp(char_ptr, char_ptr2);
    if (retval)
    {
        perror("threadFuncClone1 read data different from previous write data.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncClone1 read data same as previous write data.\n");
    }
    free(char_ptr);
    free(char_ptr2);
    pthread_exit(0);
}

void * threadFuncClone2(void *arg)
{
pthread_t *tA = (pthread_t*)arg;
    int retval;
    retval = tls_clone(*tA);
    if (retval)
    {
        perror("threadFuncClone2 tls_clone fail.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncClone2 tls_clone success.\n");
    }
    char *char_ptr = (char*)calloc(10000, sizeof(char));
    char *char_ptr2 = (char*)calloc(10000, sizeof(char));
    memset(char_ptr2, 'a', 10000);
    retval = tls_read(0, 10000, char_ptr);
    if (retval)
    {
        perror("threadFuncClone2 tls_read fail.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncClone2 tls_read success.\n");
    }
    retval = strcmp(char_ptr, char_ptr2);
    if (retval)
    {
        perror("threadFuncClone2 read data different from previous write data.\n");
        exit(-1);
    }
    else
    {
        printf("threadFuncClone2 read data same as previous write data.\n");
    }
    free(char_ptr);
    free(char_ptr2);
    pthread_exit(0);
}

int main()
{
    pthread_t tA;
    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, 2);
    pthread_create(&tA, NULL, &threadFuncA, NULL);
    pthread_barrier_wait(&barrier);
    pthread_mutex_lock(&mutex);
    int retval;
    retval = tls_clone(tA);
    if (retval)
    {
        perror("main tls_clone fail.\n");
        exit(-1);
    }
    else
    {
	printf("main tls_clone success.\n");
    }
    pthread_t tC1, tC2;
    pthread_create(&tC1, NULL, &threadFuncClone1, &tA);
    pthread_create(&tC2, NULL, &threadFuncClone2, &tA);
    char *char_ptr = (char*)calloc(10000, sizeof(char));
    retval = tls_read(0, 10000, char_ptr);
    if (retval)
    {
        perror("main tls_read fail.\n");
        exit(-1);
    }
    else
    {
	printf("main tls_read success.\n");
    }
    char *char_ptr2 = (char*)calloc(10000, sizeof(char));
    memset(char_ptr2, 'a', 10000);
    retval = strcmp(char_ptr, char_ptr2);
    if (retval)
    {
        perror("main read data different from write data.\n");
        exit(-1);
    }
    else
    {
	printf("main read data same as write data.\n");
    }
    memset(char_ptr2, 'b', 10000);
    retval = tls_write(0, 10000, char_ptr2);
    if (retval)
    {
        perror("main tls_write fail.\n");
        exit(-1);
    }
    else
    {
	printf("main tls_write success.\n");
    }
    retval = tls_read(0, 10000, char_ptr);
    if (retval)
    {
        perror("second time main tls_read fail.\n");
        exit(-1);
    }
    else
    {
	printf("second time main tls_read success.\n");
    }
    retval = strcmp(char_ptr, char_ptr2);
    if (retval)
    {
        perror("second time main read data different from write data.\n");
        exit(-1);
    }
    else
    {
	printf("second time main read data same as write data.\n");
    }
    retval = tls_destroy();
    if (retval)
    {
        perror("main tls_destroy fail.\n");
        exit(-1);
    }
    else
    {
	printf("main tls_destroy success.\n");
    }
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
    free(char_ptr);
    free(char_ptr2);
    pthread_exit(0);
}
