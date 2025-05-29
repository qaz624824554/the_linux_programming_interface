#include "tlpi_hdr.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname\n", argv[0]);

  if (unlink(argv[1]) == -1)
    errExit("unlink");

  exit(EXIT_SUCCESS);
}