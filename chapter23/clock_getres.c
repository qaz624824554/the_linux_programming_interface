#include "tlpi_hdr.h"
#include <time.h>

int main(int argc, char *argv[]) {
  struct timespec ts;

  if (clock_getres(CLOCK_REALTIME, &ts) == -1) {
    errExit("clock_getres");
  }

  printf("CLOCK_REALTIME resolution: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  if (clock_getres(CLOCK_MONOTONIC, &ts) == -1) {
    errExit("clock_getres");
  }

  printf("CLOCK_MONOTONIC resolution: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, &ts) == -1) {
    errExit("clock_getres");
  }

  printf("CLOCK_PROCESS_CPUTIME_ID resolution: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  if (clock_getres(CLOCK_THREAD_CPUTIME_ID, &ts) == -1) {
    errExit("clock_getres");
  }

  if (clock_getres(CLOCK_MONOTONIC_RAW, &ts) == -1) {
    errExit("clock_getres");
  }

  printf("CLOCK_MONOTONIC_RAW resolution: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  if (clock_getres(CLOCK_REALTIME_COARSE, &ts) == -1) {
    errExit("clock_getres");
  }

  printf("CLOCK_REALTIME_COARSE resolution: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  if (clock_getres(CLOCK_MONOTONIC_COARSE, &ts) == -1) {
    errExit("clock_getres");
  }

  printf("CLOCK_MONOTONIC_COARSE resolution: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  exit(EXIT_SUCCESS);
}