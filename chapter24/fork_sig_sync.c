/**
 * @file fork_sig_sync.c
 * @author your name (you@domain.com)
 * @brief 利用信号量实现父子进程同步
 * @version 0.1
 * @date 2025-06-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "curr_time.h"
#include "tlpi_hdr.h"
#include <signal.h>

#define SYNC_SIG SIGUSR1

static void handler(int sig) {}

int main(int argc, char *argv[]) {
  pid_t childPid;
  sigset_t blockMask, origMask, emptyMask;
  struct sigaction sa;

  setbuf(stdout, NULL);

  sigemptyset(&blockMask);
  sigaddset(&blockMask, SYNC_SIG);
  if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1) {
    errExit("sigprocmask");
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handler;
  if (sigaction(SYNC_SIG, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  switch (childPid = fork()) {
  case -1:
    errExit("fork");

  case 0:
    printf("[%s %ld] Child started - doing some work\n", currTime("%T"),
           (long)getpid());
    sleep(2);
    printf("[%s %ld] Child about to signal parent\n", currTime("%T"),
           (long)getpid());
    if (kill(getppid(), SYNC_SIG) == -1) {
      errExit("kill");
    }
    _exit(EXIT_SUCCESS);

  default:
    printf("[%s %ld] Parent about to wait for signal\n", currTime("%T"),
           (long)getpid());
    sigemptyset(&emptyMask);
    if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
      errExit("sigsuspend");
    }
    printf("[%s %ld] Parent got signal\n", currTime("%T"), (long)getpid());

    if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1) {
      errExit("sigprocmask");
    }
    exit(EXIT_SUCCESS);
  }
}