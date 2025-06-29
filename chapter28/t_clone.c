#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <sys/wait.h>

#ifndef CHILD_SIG
#define CHILD_SIG SIGUSR1
#endif

static int childFunc(void *arg) {
  if (close(*((int *)arg)) == -1) {
    errExit("close");
  }

  return 0;
}

int main(int argc, char *argv[]) {
  const int STACK_SIZE = 65536;
  char *stack;
  char *stackTop;
  int s, fd, flags;

  fd = open("/dev/null", O_RDWR);
  if (fd == -1) {
    errExit("open");
  }

  flags = (argc > 1) ? CLONE_FILES : 0;

  stack = malloc(STACK_SIZE);
  if (stack == NULL) {
    errExit("malloc");
  }

  stackTop = stack + STACK_SIZE;

  if (CHILD_SIG != 0 && CHILD_SIG != SIGCHLD) {
    if (signal(CHILD_SIG, SIG_IGN) == SIG_ERR) {
      errExit("signal");
    }
  }

  if (clone(childFunc, stackTop, flags | CHILD_SIG, &fd) == -1) {
    errExit("clone");
  }

  if (waitpid(-1, NULL, (CHILD_SIG != SIGCHLD) ? __WCLONE : 0) == -1) {
    errExit("waitpid");
  }
  printf("child has terminated\n");

  s = write(fd, "Hello, world\n", 13);
  if (s == -1) {
    errExit("write");
  }

  printf("write() on file descriptor %d succeeded\n", fd);

  exit(EXIT_SUCCESS);
}