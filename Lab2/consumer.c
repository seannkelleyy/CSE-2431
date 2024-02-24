#include <stdlib.h>
#include <unistd.h>
#include "buffer.h"

void *consumer(void *param)
{
    buffer_item item;
    unsigned int seed = time(NULL); // Seed for the random number generator

    while (1)
    {
        /* Sleep for a random period of time */
        sleep(rand_r(&seed));

        if (remove_item(&item))
        {
            printf("Error : Consumer was unable to remove item\n");
        }
        else
        {
            printf("Consumer consumed %d\n", item);
        }
    }
}
