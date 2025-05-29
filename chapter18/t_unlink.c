#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>

#define CMD_SIZE 200
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
  int fd, j , numBlocks;
  char shellCmd[CMD_SIZE];
  char buf[BUF_SIZE];

  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s temp-file [num-1k-blocks]\n", argv[0]);

  numBlocks = (argc > 2) ? getInt(argv[2], GN_GT_0, "num-1k-blocks") : 100000;

  fd = open(argv[1], O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);
  if (fd == -1)
    errExit("open");

  if (unlink(argv[1]) == -1)
    errExit("unlink");
    
  for (j = 0; j < numBlocks; j++) {
    if (write(fd, buf, BUF_SIZE) != BUF_SIZE)
      fatal("partial/failed write");
  }

  snprintf(shellCmd, CMD_SIZE, "df -k `dirname %s`", argv[1]);
  system(shellCmd);

  if (close(fd) == -1)
    errExit("close");
  printf("********** close()\n");

  system(shellCmd);
  exit(EXIT_SUCCESS);
}