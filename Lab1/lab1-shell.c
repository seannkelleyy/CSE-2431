#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a
 * null-terminated string.
 */

void setup(char inputBuffer[], char *args[], int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */

    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);

    start = -1;
    if (length == 0)
        exit(0); /* ^d was entered, end of user command stream */
    if (length < 0)
    {
        perror("error reading the command");
        exit(-1); /* terminate with error code of -1 */
    }

    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++)
    {
        switch (inputBuffer[i])
        {
        case ' ':
        case '\t': /* argument separators */
            if (start != -1)
            {
                args[ct] = &inputBuffer[start]; /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;

        case '\n': /* should be the final char examined */
            if (start != -1)
            {
                args[ct] = &inputBuffer[start];
                ct++;
            }
            inputBuffer[i] = '\0';
            args[ct] = NULL; /* no more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuffer[i] = '\0';
            break;

        default: /* some other character */
            if (start == -1)
                start = i;
        }
    }
    args[ct] = NULL; /* just in case the input line was > 80 */
}

int main(void)
{
    char inputBuffer[MAX_LINE];
    int background;
    char *args[MAX_LINE / 2 + 1];

    while (1)
    {
        background = 0;
        printf("COMMAND->");
        fflush(0);
        setup(inputBuffer, args, &background);

        if (args[0] == NULL)
        {
            continue;
        }

        if (strcmp(args[0], "cd") == 0)
        {
            if (args[1] == NULL)
            {
                fprintf(stderr, "Expected argument to \"cd\"\n");
            }
            else
            {
                if (chdir(args[1]) != 0)
                {
                    perror("Error");
                }
            }
        }
        else
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                fprintf(stderr, "Fork Failed");
                return;
            }
            else if (pid == 0)
            {
                execvp(args[0], args);
            }
            else
            {
                if (background == 0)
                {
                    while (wait(NULL) != pid)
                        ;
                }
            }
        }
    }
}
