#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include "buffer.h"
#include "buffer.c"
#include "producer.c"
#include "consumer.c"

#define MAX_SLEEP_TIME 3

pthread_mutex_t mutex;
sem_t empty, full;

int main(int argc, char *argv[])
{
    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    /* 2. Initialize buffer, mutex, semaphores, and other global vars */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    printf("sleep_time: %d\n", sleep_time);

    /* 3. Create producer thread(s) */
    pthread_t producers[num_producers];
    for (int i = 0; i < num_producers; i++)
    {
        pthread_create(&producers[i], NULL, producer, NULL);
        printf("producer %d\n", i);
    }

    /* 4. Create consumer thread(s) */
    pthread_t consumers[num_consumers];
    for (int i = 0; i < num_consumers; i++)
    {
        pthread_create(&consumers[i], NULL, consumer, NULL);
        printf("consumer %d\n", i);
    }

    /* 5. Sleep */
    sleep(sleep_time);

    /* 6. Release resources, e.g. destroy mutex and semaphores */
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    /* 7. Exit */
    return 0;
}
