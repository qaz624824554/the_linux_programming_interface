#include "tlpi_hdr.h"
#include <libgen.h>

int main(int argc, char *argv[]) {
  char *path, *dir, *base;
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s path\n", argv[0]);

  char *path_copy1 = strdup(argv[1]);
  char *path_copy2 = strdup(argv[1]);
  printf("dirname: %s\n", dirname(path_copy1));
  printf("basename: %s\n", basename(path_copy2));

  exit(EXIT_SUCCESS);
}
