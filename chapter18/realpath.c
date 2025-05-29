#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  char *resolved_path;
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname\n", argv[0]);

  resolved_path = realpath(argv[1], NULL);
  if (resolved_path == NULL)
    errExit("realpath");

  printf("Resolved path: %s\n", resolved_path);
  free(resolved_path);

  exit(EXIT_SUCCESS);
}
