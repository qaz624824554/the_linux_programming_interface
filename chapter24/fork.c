
#include <sys/types.h>
#include "tlpi_hdr.h"

static int num = 0;

int main(int argc, char *argv[]) {
  pid_t pid;
  switch (pid = fork()) {
    case -1:
      errExit("fork");
    case 0:
      num++;
      printf("child\n");
      printf("child pid: %d\n", getpid());
      printf("child ppid: %d\n", getppid());
      printf("child num: %d\n", num);
      exit(EXIT_SUCCESS);
    default:
      printf("parent\n");
      printf("parent pid: %d\n", getpid());
  }
  num += 2;
  printf("num: %d\n", num);
  printf("fork returned %d\n", pid);
  exit(EXIT_SUCCESS);
}