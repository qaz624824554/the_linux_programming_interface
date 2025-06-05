#include "tlpi_hdr.h"
#include <bits/time.h>
#include <time.h>

int main(int argc, char *argv[]) {
  struct timespec request = {2, 500000000};

  while (1) {
    if (clock_nanosleep(CLOCK_MONOTONIC, 0, &request, NULL) == -1) {
      errExit("clock_nanosleep");
    }

    printf("time is up\n");
  }

  exit(EXIT_SUCCESS);
}