#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <fcntl.h>
#include <malloc.h>

int main(int argc, char *argv[]) {
  int fd;
  ssize_t numRead;
  size_t length, alignment;
  off_t offset;
  void *buf;

  if (argc < 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s file length [offset [alignment]]\n", argv[0]);

  length = getLong(argv[2], GN_ANY_BASE, "length");
  offset = (argc > 3) ? getLong(argv[3], GN_ANY_BASE, "offset") : 0;
  alignment = (argc > 4) ? getLong(argv[4], GN_ANY_BASE, "alignment") : 4096;

  fd = open(argv[1], O_RDONLY | O_DIRECT);
  if (fd == -1)
    errExit("open");

  buf = (char *)memalign(alignment * 2, length + alignment) + alignment;
  if (buf == NULL)
    errExit("memalign");

  if (lseek(fd, offset, SEEK_SET) == -1)
    errExit("lseek");

  numRead = read(fd, buf, length);
  if (numRead == -1)
    errExit("read");

  printf("Read %ld bytes\n", (long)numRead);

  exit(EXIT_SUCCESS);
}