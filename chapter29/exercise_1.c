#include <pthread.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main() {
  printf("Hello, world\n");

  if (pthread_join(pthread_self(), NULL) != 0) {
    errExit("pthread_join");
  }

  printf("Hello, world123\n");

  return 0;
}