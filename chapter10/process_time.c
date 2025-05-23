#include "tlpi_hdr.h"
#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

static void displayProcessTime(const char *prefix) {
  struct tms t;
  clock_t clockTime;
  static long clockTicks = 0;

  if (prefix != NULL) {
    printf("%s", prefix);
  }

  if (clockTicks == 0) {
    clockTicks = sysconf(_SC_CLK_TCK);
    if (clockTicks == -1) {
      errExit("sysconf");
    }
  }

  clockTime = clock();
  if (clockTime == -1) {
    errExit("clock");
  }

  printf("     clock() returns: %ld clocks-per-sec (%.2f secs)\n",
         (long)clockTime, (double)clockTime / CLOCKS_PER_SEC);

  if (times(&t) == -1) {
    errExit("times");
  }

  printf("     times() yields: user CPU: %.2f secs, system CPU: %.2f secs\n",
         (double)t.tms_utime / clockTicks, (double)t.tms_stime / clockTicks);
}

int main(int argc, char *argv[]) {
  int numCalls, j;

  printf("CLOCKS_PER_SEC=%ld sysconf(_SC_CLK_TCK)=%ld\n\n",
         (long)CLOCKS_PER_SEC, (long)sysconf(_SC_CLK_TCK));

  displayProcessTime("At program start:\n");

  numCalls = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-calls") : 100000000;
  for (j = 0; j < numCalls; j++) {
    getppid();
  }

  displayProcessTime("After getppid() loop:\n");

  exit(EXIT_SUCCESS);
}