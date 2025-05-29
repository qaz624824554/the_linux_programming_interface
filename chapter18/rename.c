#include "tlpi_hdr.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s old-file new-file\n", argv[0]);

  if (rename(argv[1], argv[2]) == -1)
    errExit("rename");

  exit(EXIT_SUCCESS);
}