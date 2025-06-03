#include "tlpi_hdr.h"
#include <signal.h>

int main(int argc, char *argv[])
{
    printf("%s\n", strsignal(SIGINT));
    printf("%s\n", strsignal(SIGKILL));
    printf("%s\n", strsignal(SIGSEGV));
    printf("%s\n", strsignal(SIGCHLD));
    printf("%s\n", strsignal(SIGALRM));
    printf("%s\n", strsignal(SIGUSR1));
    printf("%s\n", strsignal(SIGUSR2));
    printf("%s\n", strsignal(SIGPIPE));
    printf("%s\n", strsignal(SIGTERM));
    printf("%s\n", strsignal(SIGSTKFLT));
    printf("%s\n", strsignal(SIGSTOP));
    printf("%s\n", strsignal(SIGTSTP));
    
}