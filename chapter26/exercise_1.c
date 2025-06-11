#include <signal.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  switch (fork()) {
    case -1:
      errExit("fork");
    case 0:
      // 打印pid
      printf("Child pid: %d\n", getpid());
      sleep(5);
      // 打印ppid
      printf("ppid: %d\n", getppid());
      exit(EXIT_SUCCESS);
    default:
      // 打印pid
      printf("Parent pid: %d\n", getpid());
      printf("Parent completed\n");
      exit(EXIT_SUCCESS);
  }
}