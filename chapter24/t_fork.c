#include "tlpi_hdr.h"

static int idata = 111;

int main(int argc, char *argv[]) {
  int istack = 222;
  pid_t childPid;

  switch (childPid = fork()) {
  case -1:
    errExit("fork");

  case 0:
    idata *= 3;
    istack *= 3;
    break;

  default:
    sleep(3);
    break;
  }

  printf("pid = %ld, idata = %d, istack = %d\n", (long)getpid(), idata, istack);
  exit(EXIT_SUCCESS);
}