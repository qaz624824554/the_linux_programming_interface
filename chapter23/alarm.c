#include "tlpi_hdr.h"
#include <signal.h>

static void sigHandler(int sig) {
  static int count = 0;
  if (sig == SIGALRM) {
    count++;
    printf("Caught SIGALRM (%d)\n", count);
  }
}

int main(int argc, char *argv[]) {
  signal(SIGALRM, sigHandler);

  alarm(1);

  for (;;) {
    pause();
  }

  exit(EXIT_SUCCESS);
}