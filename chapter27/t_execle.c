#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  char *envVec[] = {"GREET=hello", "LOGNAME=user", NULL};
  char *filename;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s pathname\n", argv[0]);
  }

  filename = strchr(argv[1], '/');
  if (filename != NULL) {
    filename++;
  } else {
    filename = argv[1];
  }

  execle(argv[1], filename, "hello world", (char *)NULL, envVec);
  
  printf("This should not be printed\n");

  errExit("execle");
}