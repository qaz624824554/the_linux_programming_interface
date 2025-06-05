#include "tlpi_hdr.h"
#include <stdint.h>
#include <sys/timerfd.h>
#include <time.h>

int main(int argc, char *argv[]) {
  int timerfd = timerfd_create(CLOCK_REALTIME, 0);
  if (timerfd == -1) {
    errExit("timerfd_create");
  }

  struct itimerspec its = {
      .it_value =
          {
              .tv_sec = 1,
              .tv_nsec = 0,
          },
      .it_interval =
          {
              .tv_sec = 1,
              .tv_nsec = 0,
          },
  };

  if (timerfd_settime(timerfd, TIMER_ABSTIME, &its, NULL) == -1) {
    errExit("timerfd_settime");
  }

  for (;;) {
    uint64_t exp;
    ssize_t s = read(timerfd, &exp, sizeof(uint64_t));
    if (s != sizeof(uint64_t)) {
      errExit("read");
    }
    printf("exp %llu\n", (unsigned long long)exp);
  }

  exit(EXIT_SUCCESS);
}