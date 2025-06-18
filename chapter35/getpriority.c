#include "tlpi_hdr.h"
#include <sys/resource.h>

int main(int argc, char *argv[]) {
  int nice;

  errno = 0;

  if ((nice = getpriority(PRIO_PROCESS, 0)) == -1 && errno != 0) {
    errExit("getpriority");
  }

  printf("nice value = %d\n", nice);

  exit(EXIT_SUCCESS);
}