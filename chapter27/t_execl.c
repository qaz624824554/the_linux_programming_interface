#include "tlpi_hdr.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
  printf("Initial value of USER: %s\n", getenv("USER"));
  if (putenv("USER=foo") != 0) {
    errExit("putenv");
  }

  execl("/usr/bin/printenv", "printenv", "USER", "SHELL", (char *)NULL);

  errExit("execl");
}