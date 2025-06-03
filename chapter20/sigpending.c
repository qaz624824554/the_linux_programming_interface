/**
 * @file block_sig_set.c
 * @brief 阻塞信号集
 * @version 0.1
 * @date 2025-06-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "signal_functions.h"
#include "tlpi_hdr.h"
#include <signal.h>

static sigset_t pendingSigs;

static void handler(int sig) { printf("Caught signal %d\n", sig); }

int main(int argc, char *argv[]) {
  sigset_t blockSet;

  sigemptyset(&blockSet);
  sigaddset(&blockSet, SIGINT);

  if (sigprocmask(SIG_SETMASK, &blockSet, NULL) == -1)
    errExit("sigprocmask");

  printf("Signal blocked\n");

  if (signal(SIGINT, handler) == SIG_ERR)
    errExit("signal");

  for (;;) {
    pause();

    // TODO:无法执行到这里
    printf("--------------------------------\n"); 
    if (sigpending(&pendingSigs) == -1)
      errExit("sigpending");

    printSigset(stdout, "Pending signals are: ", &pendingSigs);
  }
}