/**
 * Author: Sean Kelley
 * How to compile: You can either run `gcc -pthread -o lab2 main.c` or
 * use the `make compile` command.
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include "buffer.h"
#include "buffer.c"
#include "producer.c"
#include "consumer.c"

pthread_mutex_t mutex;
sem_t empty, full;

int main(int argc, char *argv[])
{
    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    if (argc != 4)
    {
        printf("Error: Please provide exactly 3 arguments.\n");
        return 1;
    }
    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    /* 2. Initialize buffer, mutex, semaphores, and other global vars */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    int i;

    /* 3. Create producer thread(s) */
    pthread_t producers[num_producers];
    for (i = 0; i < num_producers; i++)
    {
        if (pthread_create(&producers[i], NULL, producer, NULL) != 0)
        {
            perror("Failed to create producer thread");
            return 1;
        }
        printf("producer %d\n", i);
    }

    /* 4. Create consumer thread(s) */
    pthread_t consumers[num_consumers];
    for (i = 0; i < num_consumers; i++)
    {
        if (pthread_create(&consumers[i], NULL, consumer, NULL) != 0)
        {
            perror("Failed to create consumer thread");
            return 1;
        }
        printf("consumer %d\n", i);
    }

    for (i = 0; i < num_producers; i++)
    {
        pthread_cancel(producers[i]);
        if (pthread_join(producers[i], NULL) != 0)
        {
            perror("Failed to join producer thread");
            return 1;
        }
    }

    /* Join consumer threads */
    for (i = 0; i < num_consumers; i++)
    {
        pthread_cancel(consumers[i]);

        if (pthread_join(consumers[i], NULL) != 0)
        {
            perror("Failed to join consumer thread");
            return 1;
        }
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
