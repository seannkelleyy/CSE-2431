#include <pthread.h>
#include <stdio.h>

#define NUM_ITERATIONS 1000000

long long sharedCounter = 0;

// Mutex for synchronizing access to sharedCounter
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *incrementCounter(void *arg)
{
    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        pthread_mutex_lock(&mutex);
        sharedCounter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{
    // Create two threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, incrementCounter, NULL);
    pthread_create(&thread2, NULL, incrementCounter, NULL);

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Print the final value of sharedCounter
    printf("Final value of sharedCounter is: %lld\n", sharedCounter);

    return 0;
}
