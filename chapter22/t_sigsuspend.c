#include "signal_functions.h"
#include "tlpi_hdr.h"
#include <signal.h>
#include <string.h>
#include <time.h>

static volatile sig_atomic_t gotSigquit = 0;

static void handler(int sig) {
  printf("捕获信号 %d (%s)\n", sig, strsignal(sig));

  if (sig == SIGQUIT) {
    gotSigquit = 1;
  }
}

int main(int argc, char *argv[]) {
  int loopNum;
  time_t startTime;
  sigset_t origMask, blockMask;
  struct sigaction sa;

  printSigMask(stdout, "初始信号掩码是:\n");

  sigemptyset(&blockMask);
  sigaddset(&blockMask, SIGINT);
  sigaddset(&blockMask, SIGQUIT);

  if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) {
    errExit("sigprocmask - SIG_BLOCK");
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;

  if (sigaction(SIGINT, &sa, NULL) == -1)
    errExit("sigaction");

  if (sigaction(SIGQUIT, &sa, NULL) == -1)
    errExit("sigaction");

  for (loopNum = 1; !gotSigquit; loopNum++) {
    printf("=== 循环 %d ===\n", loopNum);
    printSigMask(stdout, "开始临界区, 信号掩码是:\n");

    for (startTime = time(NULL); time(NULL) < startTime + 4;) {
      continue;
    }

    printPendingSigs(stdout, "在 sigsuspend() 之前 - 挂起的信号:\n");

    if (sigsuspend(&origMask) == -1 && errno != EINTR) {
      errExit("sigsuspend");
    }
  }

  if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1) {
    errExit("sigprocmask - SIG_SETMASK");
  }

  printSigMask(stdout, "=== 退出循环\n恢复信号掩码为:\n");

  exit(EXIT_SUCCESS);
}