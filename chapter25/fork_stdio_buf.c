#include "tlpi_hdr.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  // fflush(stdout);
  write(STDOUT_FILENO, "Ciao\n", 5);

  if (fork() == -1) {
    errExit("fork");
  }

  exit(EXIT_SUCCESS);
}