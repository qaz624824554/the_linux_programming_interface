#include "tlpi_hdr.h"
#include <signal.h>

int main(int argc, char *argv[])
{
    psignal(SIGINT, "SIGINT");
    psignal(SIGKILL, "SIGKILL");
    psignal(SIGSEGV, "SIGSEGV");
    psignal(SIGCHLD, "SIGCHLD");
    psignal(SIGALRM, "SIGALRM");
    psignal(SIGUSR1, "SIGUSR1");
    psignal(SIGUSR2, "SIGUSR2");
}