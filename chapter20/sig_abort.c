#include "tlpi_hdr.h"
#include <signal.h>

static void sig_abort(int sig)
{
    printf("Caught signal %d\n", sig);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    printf("Parent PID=%ld\n", (long)getpid());

    if (signal(SIGABRT, sig_abort) == SIG_ERR)
        errExit("signal");

    printf("Raising SIGABRT\n");
    raise(SIGABRT);

    printf("Should not reach here\n");
}