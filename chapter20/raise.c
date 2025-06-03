#include "tlpi_hdr.h"
#include <signal.h>

static void sigHandler(int sig)
{
    printf("Ouch!\n");
}

int main(int argc, char *argv[])
{
    if (signal(SIGINT, sigHandler) == SIG_ERR)
        errExit("signal");

    raise(SIGINT);

    exit(EXIT_SUCCESS);
}
