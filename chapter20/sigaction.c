#include "tlpi_hdr.h"
#include <signal.h>
#include <stdio.h>

static void handler(int sig) { printf("Caught signal %d\n", sig); }

int main(int argc, char *argv[]) {
  struct sigaction sa;

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;

  if (sigaction(SIGINT, &sa, NULL) == -1)
    errExit("sigaction");

  for (;;) {
    printf("handler before pause\n");
    pause();
    printf("handler after pause\n");
  }

  exit(EXIT_SUCCESS);
}