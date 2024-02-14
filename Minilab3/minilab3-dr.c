#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Author: Sean Kelley
// Compiled with gcc -pthread minilab3-dr.c -o minilab3-dr
/*
Im=n my testing, I have noticed that large number of iterations
and more threads cause more significant data race issue. I have
also noticed that running 100 iterations with 2 threads also consitenly
produces 200 as the final answer. So it seems like less threads and
fewer iterations are also likely to produce to data race issues.
*/
long long sharedCounter = 0;

void *incrementCounter(void *arg)
{
    int num_iterations = *((int *)arg);
    for (int i = 0; i < num_iterations; i++)
    {
        sharedCounter++;
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
