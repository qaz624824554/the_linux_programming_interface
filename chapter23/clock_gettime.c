#include "tlpi_hdr.h"
#include <time.h>

int main(int argc, char *argv[]) {
  struct timespec ts;

  // CLOCK_REALTIME
  if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
    errExit("clock_gettime");
  }

  printf("CLOCK_REALTIME: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  // CLOCK_MONOTONIC
  if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
    errExit("clock_gettime");
  }

  printf("CLOCK_MONOTONIC: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  // CLOCK_PROCESS_CPUTIME_ID
  if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == -1) {
    errExit("clock_gettime");
  }

  printf("CLOCK_PROCESS_CPUTIME_ID: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  // CLOCK_THREAD_CPUTIME_ID
  if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) == -1) {
    errExit("clock_gettime");
  }

  printf("CLOCK_THREAD_CPUTIME_ID: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  // CLOCK_MONOTONIC_RAW
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) == -1) {
    errExit("clock_gettime");
  }
  printf("CLOCK_MONOTONIC_RAW: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  // CLOCK_REALTIME_COARSE
  if (clock_gettime(CLOCK_REALTIME_COARSE, &ts) == -1) {
    errExit("clock_gettime");
  }
  printf("CLOCK_REALTIME_COARSE: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  // CLOCK_MONOTONIC_COARSE
  if (clock_gettime(CLOCK_MONOTONIC_COARSE, &ts) == -1) {
    errExit("clock_gettime");
  }
  printf("CLOCK_MONOTONIC_COARSE: %ld.%09ld\n", (long)ts.tv_sec, ts.tv_nsec);

  exit(EXIT_SUCCESS);
}