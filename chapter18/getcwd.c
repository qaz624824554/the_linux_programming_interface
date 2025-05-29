#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  char *dirPath;
  size_t size;

  dirPath = getcwd(NULL, 0);
  if (dirPath == NULL)
    errExit("getcwd");

  printf("Current working directory = %s\n", dirPath);
  free(dirPath);

  exit(EXIT_SUCCESS);
}
