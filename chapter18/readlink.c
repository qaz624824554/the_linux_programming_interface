#include "tlpi_hdr.h"
#include <unistd.h>

#define BUF_SIZE 100000

int main(int argc, char *argv[]) {
  char buf[BUF_SIZE];
  ssize_t numRead;

  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname\n", argv[0]);

  numRead = readlink(argv[1], buf, BUF_SIZE - 1);
  if (numRead == -1)
    errExit("readlink");

  buf[numRead] = '\0';
  printf("readlink: %s\n", buf); // 打印符号链接的实际路径

  exit(EXIT_SUCCESS);
}