#include "tlpi_hdr.h"
#include <signal.h>
#include <time.h>

static int count = 0;

static void notification_handler(union sigval sv) {
  count++;
  printf("count %d\n", count);

  timer_t *timer_id = sv.sival_ptr;

  if (count == 50000) {
    struct itimerspec its = {
        .it_interval =
            {
                .tv_sec = 0,
                .tv_nsec = 1000 * 1000 * 20,
            },
        .it_value =
            {
                .tv_sec = 0,
                .tv_nsec = 1000 * 1000 * 20,
            },
    };
    if (timer_settime(*timer_id, TIMER_ABSTIME, &its, NULL) == -1) {
      errExit("timer_settime");
    }
    printf("timer_settime\n");
  } else if (count == 100000) {
    if (timer_delete(*timer_id) == -1) {
      errExit("timer_delete");
    }
    printf("timer_delete\n");
  }
}

int main(int argc, char *argv[]) {
  timer_t timer_id;
  struct sigevent sev = {
      .sigev_notify = SIGEV_THREAD,
      .sigev_notify_function = notification_handler,
      .sigev_value.sival_ptr = &timer_id,
  };

  if (timer_create(CLOCK_REALTIME, &sev, &timer_id) == -1) {
    errExit("timer_create");
  }

  struct itimerspec its = {
      .it_interval =
          {
              .tv_sec = 0,
              .tv_nsec = 1000 * 100,
          },
      .it_value =
          {
              .tv_sec = 0,
              .tv_nsec = 1000 * 100,
          },
  };

  if (timer_settime(timer_id, 0, &its, NULL) == -1) {
    errExit("timer_settime");
  }

  for (;;) {
    pause();
  }

  exit(EXIT_SUCCESS);
}