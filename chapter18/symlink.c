#include "tlpi_hdr.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname link-name\n", argv[0]);

  if (symlink(argv[1], argv[2]) == -1)
    errExit("symlink");

  exit(EXIT_SUCCESS);
}