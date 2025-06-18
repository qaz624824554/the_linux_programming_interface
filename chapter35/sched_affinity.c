#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void display_cpu_affinity(const char *msg, cpu_set_t *mask) {
  int cpu, num_cpus;

  printf("%s: ", msg);

  num_cpus = sysconf(_SC_NPROCESSORS_CONF);
  if (num_cpus == -1)
    errExit("sysconf");

  for (cpu = 0; cpu < num_cpus; cpu++) {
    printf("%d", CPU_ISSET(cpu, mask) ? 1 : 0);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  cpu_set_t mask;
  int cpu, num_cpus;
  pid_t pid;

  if (argc > 1 && strcmp(argv[1], "--help") == 0)
    usageErr("%s [pid]\n", argv[0]);

  pid = (argc > 1) ? getLong(argv[1], 0, "pid") : 0; /* 默认为当前进程 */

  /* 获取CPU数量 */
  num_cpus = sysconf(_SC_NPROCESSORS_CONF);
  if (num_cpus == -1)
    errExit("sysconf");

  printf("This system has %d processors configured.\n", num_cpus);

  /* 获取当前CPU亲和性 */
  CPU_ZERO(&mask);
  if (sched_getaffinity(pid, sizeof(mask), &mask) == -1)
    errExit("sched_getaffinity");

  display_cpu_affinity("Current CPU affinity", &mask);

  /* 修改CPU亲和性，仅使用偶数编号的CPU */
  CPU_ZERO(&mask);
  for (cpu = 0; cpu < num_cpus; cpu += 2) {
    CPU_SET(cpu, &mask);
  }

  printf("Setting CPU affinity to use only even-numbered CPUs\n");
  if (sched_setaffinity(pid, sizeof(mask), &mask) == -1)
    errExit("sched_setaffinity");

  /* 验证CPU亲和性已更改 */
  CPU_ZERO(&mask);
  if (sched_getaffinity(pid, sizeof(mask), &mask) == -1)
    errExit("sched_getaffinity");

  display_cpu_affinity("New CPU affinity", &mask);

  /* 恢复为使用所有CPU */
  CPU_ZERO(&mask);
  for (cpu = 0; cpu < num_cpus; cpu++) {
    CPU_SET(cpu, &mask);
  }

  printf("Resetting CPU affinity to use all CPUs\n");
  if (sched_setaffinity(pid, sizeof(mask), &mask) == -1)
    errExit("sched_setaffinity");

  /* 验证CPU亲和性已恢复 */
  CPU_ZERO(&mask);
  if (sched_getaffinity(pid, sizeof(mask), &mask) == -1)
    errExit("sched_getaffinity");

  display_cpu_affinity("Restored CPU affinity", &mask);

  exit(EXIT_SUCCESS);
}
