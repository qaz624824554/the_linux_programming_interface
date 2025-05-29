#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s path\n", argv[0]);

  if (chdir(argv[1]) == -1)
    errExit("chdir");

  char *cwd;
  cwd = getcwd(NULL, 0);
  if (cwd == NULL)
    errExit("getcwd");

  printf("Current working directory = %s\n", cwd);
  free(cwd);

  exit(EXIT_SUCCESS);
}