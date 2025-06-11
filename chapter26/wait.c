#include "tlpi_hdr.h"
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  pid_t pid, child_pid;
  int status;

  pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // 子进程
    printf("子进程 PID: %ld\n", (long)getpid());
    sleep(2);
    exit(EXIT_SUCCESS); // 退出状态为 42
  } else {
    // 父进程
    printf("父进程等待子进程...\n");
    child_pid = wait(&status);
    printf("子进程 %ld 已终止\n", (long)child_pid);
    printf("子进程退出状态: %d\n", status);
  }

  exit(EXIT_SUCCESS);
}