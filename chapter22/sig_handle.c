#include "tlpi_hdr.h"
#include <signal.h>

static int count = 0;

static void sig_handler(int sig, siginfo_t *info, void *ucontext) {
  printf("信号: %d\n", info->si_signo);
  printf("来源: %d\n", info->si_code);
  printf("数据: %d\n", info->si_value.sival_int);
  printf("发送者PID: %d\n", info->si_pid);
  printf("发送者UID: %d\n", info->si_uid);
  count++;
  printf("收到信号次数: %d\n", count);
}

int main(int argc, char *argv[]) {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = sig_handler;

  if (sigaction(SIGRTMIN + 1, &sa, NULL) == -1)
    errExit("sigaction");

  for (;;)
    pause();
  exit(EXIT_SUCCESS);
}