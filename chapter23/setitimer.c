#include "tlpi_hdr.h"
#include <signal.h>
#include <sys/time.h>

static void sigHandler(int sig) {
  static int count = 0;
  if (sig == SIGALRM) {
    count++;
    printf("Caught SIGALRM (%d)\n", count);
  }
}

int main(int argc, char *argv[]) {

  signal(SIGALRM, sigHandler);

  struct itimerval itv;
  struct itimerval oitv;

  itv.it_value.tv_sec = 1;
  itv.it_value.tv_usec = 0;
  itv.it_interval.tv_sec = 1;
  itv.it_interval.tv_usec = 0;

  if (setitimer(ITIMER_REAL, &itv, &oitv) == -1) {
    errExit("setitimer");
  }

  for (;;) {
    pause();
  }

  exit(EXIT_SUCCESS);
}