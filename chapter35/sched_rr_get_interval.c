#include "tlpi_hdr.h"
#include <sched.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  struct timespec tp;
  pid_t pid;
  int s;

  if (argc > 1 && strcmp(argv[1], "--help") == 0)
    usageErr("%s [pid]\n", argv[0]);

  pid = (argc > 1) ? getLong(argv[1], 0, "pid") : 0; /* 默认为当前进程 */

  /* 获取进程的时间片长度 */
  s = sched_rr_get_interval(pid, &tp);
  if (s == -1)
    errExit("sched_rr_get_interval");

  printf("Round Robin 时间片: %ld.%09ld seconds\n", (long)tp.tv_sec,
         tp.tv_nsec);

  /* 获取并显示当前进程的调度策略 */
  s = sched_getscheduler(pid);
  if (s == -1)
    errExit("sched_getscheduler");

  printf("当前调度策略: ");
  switch (s) {
  case SCHED_OTHER:
    printf("SCHED_OTHER\n");
    break;
  case SCHED_FIFO:
    printf("SCHED_FIFO\n");
    break;
  case SCHED_RR:
    printf("SCHED_RR\n");
    break;
  default:
    printf("%d\n", s);
    break;
  }

  /* 如果不是 SCHED_RR，打印提示信息 */
  if (s != SCHED_RR) {
    printf("注意: 此进程不是使用 SCHED_RR 策略调度的。\n");
    printf("即使如此，系统仍然会返回一个假设的时间片值。\n");
  }

  exit(EXIT_SUCCESS);
}
