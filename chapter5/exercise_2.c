#include "../lib/tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  int fd;

  if ((fd = open(argv[1], O_RDWR | O_APPEND)) == -1) {
    errExit("open");
  }

  if (lseek(fd, 0, SEEK_SET) == -1) {
    errExit("lseek");
  }

  if (write(fd, "Hello, World!\n", 14) == -1) {
    errExit("write");
  }

  if (close(fd) == -1) {
    errExit("close");
  }
}