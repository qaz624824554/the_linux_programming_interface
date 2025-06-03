#include "tlpi_hdr.h"
#include <signal.h>

int main(int argc, char *argv[]) {
  sigset_t sigset;

  if (sigemptyset(&sigset) == -1)
    errExit("sigemptyset");

  if (sigaddset(&sigset, SIGINT) == -1)
    errExit("sigaddset");

  if (sigaddset(&sigset, SIGCHLD) == -1)
    errExit("sigaddset");

  if (sigismember(&sigset, SIGINT))
    printf("SIGINT is a member\n");

  if (sigismember(&sigset, SIGCHLD))
    printf("SIGCHLD is a member\n");
}