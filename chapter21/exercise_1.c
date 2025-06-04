/**
 * @file exercise_1.c
 * @author your name (you@domain.com)
 * @brief 实现 abort 函数
 * @version 0.1
 * @date 2025-06-04
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void abort_handler(int sig) {
  printf("abort_handler: %d\n", sig);
  exit(1);
}

void abort(void) {
  struct sigaction sa;
  sa.sa_handler = abort_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGABRT, &sa, NULL);
  raise(SIGABRT);
  exit(1);
}

int main(int argc, char *argv[]) {
  struct sigaction sa;
  sa.sa_handler = abort_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGABRT, &sa, NULL);

  abort();

  return 0;
}