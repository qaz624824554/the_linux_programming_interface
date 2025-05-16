#include "../lib/tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>

int main(int argc, char *argv[]) {
  int fd1,fd2,fd3;

  if (argc < 2) {
    errExit("argc");
  }

  fd1 = open(argv[1], O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
  fd2 = dup(fd1);
  fd3 = open(argv[1], O_RDWR);

  write(fd1, "Hello,", 6);
  write(fd2, "world\n", 6);
  lseek(fd2, 0, SEEK_SET);
  write(fd1, "HELLO,", 6);
  write(fd3, "Gidday", 6);
}