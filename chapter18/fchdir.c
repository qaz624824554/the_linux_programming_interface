#include "tlpi_hdr.h"
#include <fcntl.h>
int main(int argc, char *argv[]) {
  int fddir;

  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s path\n", argv[0]);

  fddir = open(argv[1], O_RDONLY);
  if (fddir == -1)
    errExit("open");

  if (fchdir(fddir) == -1)
    errExit("fchdir");

  char *cwd;
  cwd = getcwd(NULL, 0);
  if (cwd == NULL)
    errExit("getcwd");

  printf("Current working directory = %s\n", cwd);
  free(cwd);

  exit(EXIT_SUCCESS);
}