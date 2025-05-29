#define _XOPEN_SOURCE 700

#include "tlpi_hdr.h"
#include <fcntl.h>
int main(int argc, char *argv[]) {
  int fd;
  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname\n", argv[0]);

  fd = open(argv[1], O_RDONLY);
  if (fd == -1)
    errExit("open");

  if (openat(fd, argv[2], O_RDONLY) == -1)
    errExit("openat");

  exit(EXIT_SUCCESS);
}