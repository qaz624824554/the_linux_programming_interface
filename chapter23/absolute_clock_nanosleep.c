#include "tlpi_hdr.h"
#include <time.h>

int main(int argc, char *argv[]) {
  struct timespec target_time;
  clock_gettime(CLOCK_MONOTONIC, &target_time);
  target_time.tv_sec += 10; // 10秒后

  while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &target_time, NULL) ==
         -1) {
    if (errno == EINTR) {
      // 被信号中断，重启时使用相同的绝对时间
      continue;
    } else {
      perror("clock_nanosleep");
      break;
    }
  }

  printf("clock_nanosleep completed\n");

  exit(EXIT_SUCCESS);
}