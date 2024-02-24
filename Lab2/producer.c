#include <stdio.h>
#include <unistd.h>
#include "buffer.h"

void *producer(void *param)
{
    buffer_item rand;
    unsigned int seed = time(NULL); // Seed for the random number generator

    while (1)
    {
        /* Sleep for a random period of time */
        sleep(rand_r(&seed) % 10);

        /* Generate a random number */
        rand = rand_r(&seed) % 3;

        if (insert_item(rand) == -1)
        {
            printf("Error : Producer was unable to insert item %d\n", rand);
        }
        else
        {
            printf("Producer produced %d\n", rand);
        }
    }
}