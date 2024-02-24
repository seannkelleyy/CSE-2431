#include <stdio.h>
#include <unistd.h>
#include "buffer.h"

void *producer(void *param)
{
    buffer_item item;
    unsigned int seed = time(NULL); // Seed for the random number generator

    while (1)
    {
        /* Sleep for a random period of time */
        sleep(rand_r(&seed));

        /* Generate a random number */
        item = rand_r(&seed);

        if (insert_item(item))
        {
            printf("Error : Producer was unable to insert item %d\n", item);
        }
        else
        {
            printf("Producer produced %d\n", item);
        }
    }
}