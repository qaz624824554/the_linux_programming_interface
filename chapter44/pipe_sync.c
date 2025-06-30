#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_CHILDREN 3

int main() {
  int pipefd[2];
  pid_t children[NUM_CHILDREN];
  int i;
  char buf;

  printf("=== 演示使用管道同步多个进程 ===\n");
  printf("父进程将创建 %d 个子进程，并等待它们全部完成工作\n\n", NUM_CHILDREN);

  /* ① 创建管道 */
  if (pipe(pipefd) == -1) {
    perror("pipe 创建失败");
    exit(EXIT_FAILURE);
  }

  /* ② 创建多个子进程 */
  for (i = 0; i < NUM_CHILDREN; i++) {
    children[i] = fork();

    if (children[i] == -1) {
      perror("fork 失败");
      exit(EXIT_FAILURE);
    }

    if (children[i] == 0) {
      /* 子进程 */
      printf("子进程 %d (PID: %d) 开始工作...\n", i + 1, getpid());

      /* 模拟一些工作 - 睡眠不同的时间 */
      sleep((i + 1) * 2);

      printf("子进程 %d (PID: %d) 完成工作\n", i + 1, getpid());

      /* ③ 子进程完成工作后关闭管道写入端 */
      close(pipefd[1]);
      close(pipefd[0]); /* 也关闭读取端，虽然子进程不使用 */

      _exit(EXIT_SUCCESS);
    }
  }

  /* 父进程 */
  /* ④ 父进程关闭管道的写入端（重要！） */
  close(pipefd[1]);

  printf("父进程已创建所有子进程，现在等待它们完成...\n");
  printf("父进程正在管道上执行阻塞读取...\n\n");

  /* ⑤ 父进程在管道上读取，直到所有子进程都关闭了写入端 */
  /* 当所有子进程都关闭写入端后，read() 将返回 0 (EOF) */
  while (read(pipefd[0], &buf, 1) > 0) {
    /* 这个循环不应该执行，因为没有子进程向管道写入数据 */
    /* 但我们保留它以防万一 */
  }

  printf("管道读取结束 - 所有子进程都已关闭管道写入端\n");
  printf("父进程知道所有子进程都已完成工作！\n\n");

  /* 关闭管道读取端 */
  close(pipefd[0]);

  /* 等待所有子进程正式终止（清理僵尸进程） */
  printf("父进程开始回收子进程资源...\n");
  for (i = 0; i < NUM_CHILDREN; i++) {
    int status;
    pid_t terminated_pid = waitpid(children[i], &status, 0);

    if (terminated_pid > 0) {
      printf("回收子进程 %d (PID: %d)，退出状态: %d\n", i + 1, terminated_pid,
             WEXITSTATUS(status));
    }
  }

  printf("\n所有子进程已完成并被回收\n");
  printf("父进程可以继续执行其他工作...\n");
  printf("同步演示完成！\n");

  return 0;
}
