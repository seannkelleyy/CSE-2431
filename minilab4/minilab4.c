#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

volatile sig_atomic_t ctrl_c_count = 0;

void handle_sigint(int sig)
{
    ctrl_c_count++;
}

void handle_sigquit(int sig)
{
    printf("Ctrl+C was pressed %d times\n", ctrl_c_count);
    exit(0);
}

int main()
{
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    while (1)
    {
        // makes program run indefinitely, so you can keep making signals
    }
    return 0;
}