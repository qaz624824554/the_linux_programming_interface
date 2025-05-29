#include "tlpi_hdr.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc > 1 && strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname\n", argv[0]);

  if (rmdir(argv[1]) == -1)
    errExit("rmdir");

  exit(EXIT_SUCCESS);
}