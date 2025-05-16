#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int fd;
  char template[] = "/tmp/somestringXXXXXX";

  fd = mkstemp(template);
  if (fd == -1) {
    perror("mkstemp");
    exit(EXIT_FAILURE);
  }

  printf("Generated filename was: %s\n", template);
  unlink(template);

  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}