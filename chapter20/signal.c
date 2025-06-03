#include "tlpi_hdr.h"
#include <signal.h>

static void sigHandler(int sig) {
  printf("Caught signal %d\n", sig);
}

int main(int argc, char *argv[]) {
  if (signal(SIGINT, sigHandler) == SIG_ERR) {
    errExit("signal");
  }

  for (;;) {
    pause();
  }

  exit(EXIT_SUCCESS);
}
