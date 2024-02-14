#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Author: Sean Kelley
// Compiled with gcc -pthread minilab3-fixed.c -o minilab3-fixed

long long sharedCounter = 0;

// Mutex for synchronizing access to sharedCounter
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *incrementCounter(void *arg)
{
    int num_iterations = *((int *)arg);
    for (int i = 0; i < num_iterations; i++)
    {
        pthread_mutex_lock(&mutex);
        sharedCounter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <#_iteration> <#_thread>\n", argv[0]);
        return 1;
    }

    int num_iterations = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    if (num_threads > 10)
    {
        printf("The maximal number of threads is 10.\n");
        return 1;
    }

    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, incrementCounter, &num_iterations);
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Final value of sharedCounter is: %lld\n", sharedCounter);

    return 0;
}
