/**
 * @file exercise_2.c
 * @author your name (you@domain.com)
 * @brief 实时信号先于标准信号传递，标准信号发送多个会被合并
 * @version 0.1
 * @date 2025-06-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "tlpi_hdr.h"
#include <signal.h>

static void handler(int sig) {
  printf("捕获信号 %d (%s)\n", sig, strsignal(sig));
}

int main(int argc, char *argv[]) {
  sigset_t allSigs;
  struct sigaction sa;
  int sig;

  printf("程序 PID: %ld\n", (long)getpid());
  printf("阻塞所有信号，请在另一个终端发送各种信号...\n");

  // 设置信号处理器
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;

  // 为所有信号设置处理器（除了不能捕获的信号）
  for (sig = 1; sig < NSIG; sig++) {
    if (sig != SIGKILL && sig != SIGSTOP) {
      sigaction(sig, &sa, NULL);
    }
  }

  // 阻塞所有信号
  sigfillset(&allSigs);
  sigdelset(&allSigs, SIGKILL);
  sigdelset(&allSigs, SIGSTOP);

  if (sigprocmask(SIG_SETMASK, &allSigs, NULL) == -1) {
    errExit("sigprocmask");
  }

  printf("所有信号已阻塞，等待 10 秒钟...\n");
  printf("请发送标准信号（如 SIGTERM, SIGINT）和实时信号（如 SIGRTMIN, "
         "SIGRTMAX）\n");

  // 等待一段时间让用户发送信号
  sleep(40);

  printf("开始解除信号阻塞，观察信号传递顺序...\n");

  // 解除所有信号的阻塞
  sigemptyset(&allSigs);
  if (sigprocmask(SIG_SETMASK, &allSigs, NULL) == -1) {
    errExit("sigprocmask");
  }

  printf("信号阻塞已解除\n");

  // 等待一下让信号处理完成
  sleep(2);

  printf("程序结束\n");
  exit(EXIT_SUCCESS);
}
