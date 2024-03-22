/*
Author: Sean Kelley
How to compile: gcc minilab5.c -o minilab5
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the maximum number of frames
#define MAX_FRAMES 20

// Define the maximum length of the reference string
#define MAX_REFSTRING_LENGTH 80

// Define the policy IDs
#define OPTIMAL 0
#define FIFO 1
#define LRU 2

// Function prototypes
void optimal(int num_frames, char *page_ref_string);
void fifo(int num_frames, char *page_ref_string);
void lru(int num_frames, char *page_ref_string);

int main(int argc, char *argv[])
{
    // Check the number of arguments
    if (argc != 4)
    {
        printf("Usage: ./minilab5 num_frames policy_id refstring\n");
        return 1;
    }

    // Get the number of frames and the policy ID from the command line arguments
    int num_frames = atoi(argv[1]);
    int policy_id = atoi(argv[2]);
    char refstring[MAX_REFSTRING_LENGTH];

    // Try to open the third argument as a file
    FILE *file = fopen(argv[3], "r");
    if (file != NULL)
    {
        // If the file opens successfully, read the reference string from the file
        if (fgets(refstring, MAX_REFSTRING_LENGTH, file) == NULL)
        {
            printf("Error reading file.\n");
            fclose(file);
            return 1;
        }
        fclose(file);
    }
    else
    {
        // If the file does not open, treat the third argument as the reference string
        strncpy(refstring, argv[3], MAX_REFSTRING_LENGTH - 1);
        refstring[MAX_REFSTRING_LENGTH - 1] = '\0'; // Ensure null-termination
    }

    // Run the appropriate policy
    switch (policy_id)
    {
    case OPTIMAL:
        optimal(num_frames, refstring);
        break;
    case FIFO:
        fifo(num_frames, refstring);
        break;
    case LRU:
        lru(num_frames, refstring);
        break;
    default:
        printf("Invalid policy ID.\n");
        return 1;
    }

    return 0;
}

void optimal(int num_frames, char *refstring)
{
    char frame[num_frames];
    int hit = 0, age[num_frames];
    char pages[256];
    int pn = strlen(refstring);
    for (int i = 0; i < pn; i++)
        pages[i] = refstring[i];
    for (int i = 0; i < num_frames; i++)
    {
        frame[i] = -1;
        age[i] = -1;
    }
    for (int i = 0; i < pn; i++)
    {
        int j;
        for (j = 0; j < num_frames; j++)
            if (frame[j] == pages[i])
            {
                hit++;
                age[j] = i;
                break;
            }
        if (j == num_frames)
        {
            int pos = 0, farthest = -1;
            for (j = 0; j < num_frames; j++)
            {
                int k;
                for (k = i + 1; k < pn; k++)
                {
                    if (frame[j] == pages[k])
                        break;
                }
                if (k == pn)
                {
                    pos = j;
                    break;
                }
                else if (k > farthest)
                {
                    farthest = k;
                    pos = j;
                }
            }
            frame[pos] = pages[i];
            age[pos] = i;
        }
    }
    printf("Optimal: length: %d, hits: %d, faults: %d\n", pn, hit, pn - hit);
    printf("Final state of memory content: ");
    for (int i = 0; i < num_frames; i++)
        printf("%c ", frame[i]);
    printf("\n");
}

void fifo(int num_frames, char *refstring)
{
    int frame[num_frames], hit = 0, f = 0;
    int pages[256];
    int pn = strlen(refstring);
    for (int i = 0; i < pn; i++)
        pages[i] = refstring[i];
    for (int i = 0; i < num_frames; i++)
        frame[i] = -1;
    for (int i = 0; i < pn; i++)
    {
        int j;
        for (j = 0; j < num_frames; j++)
            if (frame[j] == pages[i])
            {
                hit++;
                break;
            }
        if (j == num_frames)
        {
            frame[f] = pages[i];
            f = (f + 1) % num_frames;
        }
    }
    printf("FIFO: length: %d, hits: %d, faults: %d\n", pn, hit, pn - hit);
    printf("Final state of memory content: ");
    for (int i = 0; i < num_frames; i++)
        printf("%c ", frame[i]);
    printf("\n");
}

void lru(int num_frames, char *refstring)
{
    int frame[num_frames], hit = 0, age[num_frames];
    int pages[256];
    int pn = strlen(refstring);
    for (int i = 0; i < pn; i++)
        pages[i] = refstring[i];
    for (int i = 0; i < num_frames; i++)
    {
        frame[i] = -1;
        age[i] = -1;
    }
    for (int i = 0; i < pn; i++)
    {
        int j;
        for (j = 0; j < num_frames; j++)
            if (frame[j] == pages[i])
            {
                hit++;
                age[j] = i;
                break;
            }
        if (j == num_frames)
        {
            int min = age[0], pos = 0;
            for (j = 1; j < num_frames; j++)
                if (age[j] < min)
                {
                    min = age[j];
                    pos = j;
                }
            frame[pos] = pages[i];
            age[pos] = i;
        }
    }
    printf("LRU: length: %d, hits: %d, faults: %d\n", pn, hit, pn - hit);
    printf("Final state of memory content: ");
    for (int i = 0; i < num_frames; i++)
        printf("%c ", frame[i]);
    printf("\n");
}
