#include "tlpi_hdr.h"
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int policy, old_policy;
  struct sched_param sp;
  pid_t pid;

  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s policy priority [pid]\n"
             "    policy: 0=SCHED_OTHER, 1=SCHED_FIFO, 2=SCHED_RR\n"
             "    priority: 0..%d for SCHED_OTHER, %d..%d for SCHED_FIFO/RR\n"
             "    pid: defaults to current process if omitted\n",
             argv[0], 0, sched_get_priority_min(SCHED_FIFO),
             sched_get_priority_max(SCHED_FIFO));
  }

  /* 解析命令行参数 */
  policy = getInt(argv[1], 0, "policy");

  if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR) {
    cmdLineErr("Invalid policy\n");
  }

  sp.sched_priority = getInt(argv[2], 0, "priority");

  pid = (argc > 3) ? getLong(argv[3], 0, "pid") : 0; /* 默认为当前进程 */

  /* 获取当前调度策略 */
  old_policy = sched_getscheduler(pid);
  if (old_policy == -1)
    errExit("sched_getscheduler");

  printf("Old scheduling policy: ");
  switch (old_policy) {
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
    printf("%d\n", old_policy);
    break;
  }

  /* 设置新的调度策略和优先级 */
  if (sched_setscheduler(pid, policy, &sp) == -1)
    errExit("sched_setscheduler");

  /* 验证调度策略已更改 */
  policy = sched_getscheduler(pid);
  if (policy == -1)
    errExit("sched_getscheduler");

  printf("New scheduling policy: ");
  switch (policy) {
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
    printf("%d\n", policy);
    break;
  }

  /* 获取并显示当前优先级 */
  if (sched_getparam(pid, &sp) == -1)
    errExit("sched_getparam");

  printf("New priority: %d\n", sp.sched_priority);

  exit(EXIT_SUCCESS);
}
