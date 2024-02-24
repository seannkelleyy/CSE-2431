#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

pthread_mutex_t mutex;
sem_t empty, full;

buffer_item buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

int insert_item(buffer_item item)
{
    /* insert an item into buffer */
    /* return 0 if successful, otherwise return -1 indicating an error condition */
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
    sem_post(&full);

    printf("Producer produced %d\n", item);
    return 0;
}

int remove_item(buffer_item *itemp)
{
    /* remove an object from buffer and placing it in itemp */
    /* return 0 if successful, otherwise return -1 indicating an error condition */
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    *itemp = buffer[out];
    out = (out + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);

    printf("Consumer consumed %d\n", *itemp);
    return 0;
}
