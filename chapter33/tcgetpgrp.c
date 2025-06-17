#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  int fd;
  pid_t pgrp;

  fd = open("/dev/tty", O_RDONLY);
  if (fd == -1)
    errExit("open");

  pgrp = tcgetpgrp(fd);
  if (pgrp == -1)
    errExit("tcgetpgrp");

  printf("当前进程的前台进程组ID: %ld\n", (long)pgrp);

  close(fd);
  exit(EXIT_SUCCESS);
}