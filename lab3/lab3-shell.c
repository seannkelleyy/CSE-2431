/* Author: Sean Kelley
    How to compile: After saving the lab1-shell.c file, go to the
    directory where is was saved and run the command: gcc -o lab1 lab1-shell.c
    After this you should be able to run the program by typing ./lab1 or lab1,
    depending on your system.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_LINE 80
#define MAX_HISTORY 10

char *history[MAX_HISTORY];
int HISTORY_COUNT = 0;
int history_index = 0;

void add_to_history(char *cmd)
{
    if (cmd[0] == 'r' && (cmd[1] == '\0' || cmd[1] == ' '))
    {
        return;
    }

    if (HISTORY_COUNT == MAX_HISTORY)
    {
        free(history[0]); // Free the oldest command
        for (int i = 0; i < MAX_HISTORY - 1; i++)
        {
            history[i] = history[i + 1];
        }
        history[MAX_HISTORY - 1] = strdup(cmd); // Add the new command
    }
    else
    {
        history[HISTORY_COUNT++] = strdup(cmd); // Add the new command
    }

    printf("Added to history: %s\n", cmd);
}

void handle_SIGINT(int signum)
{
    printf("\n");
    for (int i = 0; i < HISTORY_COUNT; i++)
    {
        printf("%d %s\n", i + 1, history[i]);
    }
    printf("COMMAND->");
    fflush(stdout);
}

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
    do
    {
        length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    } while (length < 0 && errno == EINTR);

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

        default:
            if (start == -1)
                start = i;
        }
    }
    args[ct] = NULL; /* just in case the input line was > 80 */
}

void execute_command(char *args[], int background)
{
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
        return; // Don't fork for cd command
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Fork Failed");
        exit(1);
    }
    else if (pid == 0)
    {
        execvp(args[0], args);
        perror("Error");
        exit(EXIT_FAILURE);
    }
    else
    {
        if (background == 0)
        {
            waitpid(pid, NULL, 0);
        }
    }
}

int main(void)
{
    char inputBuffer[MAX_LINE];
    int background;
    char *args[MAX_LINE / 2 + 1];

    struct sigaction handler = {.sa_handler = handle_SIGINT, .sa_flags = SA_RESTART};
    sigaction(SIGINT, &handler, NULL);

    while (1)
    {
        background = 0;
        printf("COMMAND->");
        fflush(stdout);
        setup(inputBuffer, args, &background);

        if (args[0] == NULL)
        {
            continue;
        }

        if (strcmp(args[0], "r") == 0)
        {
            char *cmd = NULL;
            if (args[1] == NULL || args[1][0] == '\n')
            { // "r" command or "r\n"
                if (HISTORY_COUNT == 0)
                {
                    fprintf(stderr, "No commands in history.\n");
                    continue;
                }
                else
                {
                    cmd = strdup(history[(history_index - 1 + MAX_HISTORY) % MAX_HISTORY]);
                }
            }
            else if (args[1][1] == '\0')
            { // "r x" command
                for (int i = HISTORY_COUNT - 1; i >= 0; i--)
                {
                    int index = (history_index - 1 - i + MAX_HISTORY) % MAX_HISTORY;
                    if (history[index][0] == args[1][0])
                    {
                        cmd = strdup(history[index]);
                        break;
                    }
                }
                if (cmd == NULL)
                {
                    fprintf(stderr, "No matching commands in history.\n");
                    continue;
                }
            }
            else
            {
                fprintf(stderr, "Invalid command format.\n");
                continue;
            }

            if (cmd)
            {
                if (strlen(cmd) >= MAX_LINE)
                {
                    fprintf(stderr, "Command exceeds maximum length.\n");
                    free(cmd);
                    continue;
                }
                strcpy(inputBuffer, cmd);
                setup(inputBuffer, args, &background);
                free(cmd);
            }
        }

        execute_command(args, background);
        add_to_history(inputBuffer);
        free(history[history_index]);                 // Free allocated memory for old command
        history[history_index] = strdup(inputBuffer); // Add the new command
        history_index = (history_index + 1) % MAX_HISTORY;
    }

    return 0;
}