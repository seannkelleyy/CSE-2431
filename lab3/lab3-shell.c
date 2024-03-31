#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

/* Author: Sean Kelley
    How to compile: After saving the lab3-shell.c file, go to the
    directory where is was saved and run the command: gcc -o lab3 lab3-shell.c
    After this you should be able to run the program by typing ./lab3 or lab3,
    depending on your system.
*/

#define MAX_LINE 80
#define MAX_HISTORY 10

char *history[MAX_HISTORY];
int HISTORY_COUNT = 0;
int HISTORY_INDEX = 0;

void split_args(char *args[])
{
    char *cmd = strdup(args[0]);
    if (cmd == NULL)
    {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        return;
    }

    char *token = strtok(cmd, " ");
    int i = 0;

    while (token != NULL)
    {
        char *arg = strdup(token);
        if (arg == NULL)
        {
            fprintf(stderr, "Error: Failed to allocate memory\n");
            break;
        }
        args[i] = arg;
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;

    free(cmd);
}

void add_to_history(char *cmd)
{
    // Avoid adding 'r' command to history
    if (strcmp(cmd, "r") == 0)
    {
        return;
    }

    // Free the old command if it exists
    free(history[HISTORY_INDEX]);

    // Add the new command to history
    history[HISTORY_INDEX] = strdup(cmd);

    // Update history index and count
    HISTORY_INDEX = (HISTORY_INDEX + 1) % MAX_HISTORY;
    if (HISTORY_COUNT < MAX_HISTORY)
    {
        HISTORY_COUNT++;
    }
}

char *retrieve_command(int index)
{
    if (index < 0 || index > HISTORY_COUNT)
    {
        fprintf(stderr, "Index out of bounds.\n");
        return NULL;
    }

    return history[index - 1];
}

void handle_SIGINT(int signum)
{
    printf("\n");
    for (int i = 0; i < HISTORY_COUNT; i++)
    {
        if (history[i] != NULL)
        {
            printf("%d: %s\n", i + 1, history[i]);
        }
    }
    printf("COMMAND->");
    fflush(stdout);
}

void setup(char inputBuffer[], char *args[], int *background)
{
    int length,
        i = 0,
        start = -1,
        ct = 0;

    /* read what the user enters on the command line */
    do
    {
        length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    } while (length < 0 && errno == EINTR);

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

int execute_command(char *args[], int background)
{
    if (args[0] == NULL)
    {
        printf("No command entered\n");
        return -1;
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
                perror("Error changing directory");
            }
        }
        return 0; // Don't fork for cd command
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Fork Failed");
        return -1;
    }
    else if (pid == 0) // This is the child process
    {
        if (execvp(args[0], args) == -1)
        {
            fprintf(stderr, "Error: Invalid command '%s'\n", args[0]);
            _exit(EXIT_FAILURE); // Exit the child process
        }
    }
    else // This is the parent process
    {
        if (background == 0)
        {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
            {
                return -1;
            }
        }
    }
    return 0;
}

int main(void)
{
    char inputBuffer[MAX_LINE];
    int background;
    char *args[MAX_LINE / 2 + 1];

    // Set up the signal handler
    struct sigaction handler = {.sa_handler = handle_SIGINT, .sa_flags = SA_RESTART};
    sigaction(SIGINT, &handler, NULL);
    strcpy(inputBuffer, "Caught <ctrl><c>\n");

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

        /* If the command is "r" or "r x", replace it with the command from history. */
        if (strcmp(args[0], "r") == 0)
        {
            char *cmd = NULL;
            if (args[1] == NULL) /* "r" command */
            {
                if (HISTORY_COUNT == 0)
                {
                    fprintf(stderr, "No commands in history.\n");
                    continue;
                }
                else
                {
                    cmd = retrieve_command(HISTORY_COUNT); // Retrieve the most recent command
                    if (cmd != NULL)
                    {
                        args[0] = strdup(cmd);
                    }
                }
            }
            else if (args[1][1] == '\0') /* "r x" command */
            {
                int i;
                for (i = HISTORY_COUNT - 1; i >= 0; i--)
                {
                    if (history[i][0] == args[1][0])
                    {
                        args[1] = NULL;
                        cmd = retrieve_command(i + 1);
                        if (cmd != NULL)
                        {
                            args[0] = strdup(cmd);
                        }
                        break;
                    }
                }
                if (i < 0)
                {
                    fprintf(stderr, "No matching commands in history.\n");
                    continue;
                }
            }
        }
        if (args[0] == NULL)
        {
            break;
        }

        // Split the command into arguments if it contains spaces
        if (strchr(args[0], ' ') != NULL)
        {
            split_args(args);
        }

        if (execute_command(args, background) == -1)
        {
            continue;
        }
        else
        {
            char *command = strdup(args[0]);

            /* Add a space and the second argument, if it exists. */
            if (args[1] != NULL)
            {
                command = realloc(command, strlen(command) + strlen(args[1]) + 2);
                strcat(command, " ");
                strcat(command, args[1]);
            }
            add_to_history(command);

            free(command);

            if (history[HISTORY_INDEX] != NULL)
            {
                free(history[HISTORY_INDEX]);
            }
            continue;
        }

        return 0;
    }
}