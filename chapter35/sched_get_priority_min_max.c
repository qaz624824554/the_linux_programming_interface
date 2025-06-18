#include <sched.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  int min, max;

  min = sched_get_priority_min(SCHED_FIFO);
  max = sched_get_priority_max(SCHED_FIFO);

  printf("SCHED_FIFO priority range: %d to %d\n", min, max);

  min = sched_get_priority_min(SCHED_RR);
  max = sched_get_priority_max(SCHED_RR);

  printf("SCHED_RR priority range: %d to %d\n", min, max);

  min = sched_get_priority_min(SCHED_OTHER);
  max = sched_get_priority_max(SCHED_OTHER);

  printf("SCHED_OTHER priority range: %d to %d\n", min, max);

  exit(EXIT_SUCCESS);
}