/**
 * @file block_sig_set.c
 * @brief 阻塞信号集
 * @version 0.1
 * @date 2025-06-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "tlpi_hdr.h"
#include <signal.h>

static void handler(int sig) { printf("Caught signal %d\n", sig); }

int main(int argc, char *argv[]) {
  sigset_t blockSet;

  sigfillset(&blockSet);

  if (sigprocmask(SIG_SETMASK, &blockSet, NULL) == -1)
    errExit("sigprocmask");

  printf("Signal blocked\n");

  if (signal(SIGINT, handler) == SIG_ERR)
    errExit("signal");

  for (;;) {
    pause();
  }
}