#include "tlpi_hdr.h"
#include <libgen.h>
#include <signal.h>
#include <sys/wait.h>

#define CMD_SIZE 200

int main(int argc, char *argv[]) {
  char cmd[CMD_SIZE]; /* 用于存储ps命令的缓冲区 */
  pid_t childPid;     /* 存储子进程的PID */

  /* 禁用stdout缓冲，确保输出立即显示 */
  setbuf(stdout, NULL);

  /* 打印父进程的PID */
  printf("Parent PID=%ld\n", (long)getpid());

  /* 创建子进程并根据返回值执行不同的代码分支 */
  switch (childPid = fork()) {
  case -1: {
    /* fork()失败的情况 */
    errExit("fork");
  }
  case 0: {
    /* 子进程执行的代码 */
    printf("Child (PID=%ld) exiting\n", (long)getpid());
    /* 子进程立即退出，成为僵尸进程（因为父进程没有立即wait） */
    _exit(EXIT_SUCCESS);
  }
  default: {
    /* 父进程执行的代码 */

    /* 等待3秒，让子进程有时间退出并成为僵尸进程 */
    sleep(3);

    /* 构造ps命令来查看当前程序的进程状态 */
    snprintf(cmd, CMD_SIZE, "ps aux | grep %s", basename(argv[0]));
    cmd[CMD_SIZE - 1] = '\0'; /* 确保字符串以null结尾 */

    /* 执行ps命令，此时应该能看到僵尸进程（状态为Z） */
    system(cmd);

    /* 尝试向僵尸进程发送SIGKILL信号 */
    if (kill(childPid, SIGKILL) == -1) {
      errMsg("kill");
    }

    /* 再等待3秒 */
    sleep(3);

    /* 打印提示信息并再次执行ps命令 */
    printf("After sending SIGKILL to zombie (PID=%ld):\n", (long)childPid);
    /* 这次执行ps命令应该仍然能看到僵尸进程，因为SIGKILL无法杀死僵尸进程 */
    /* 僵尸进程只能通过父进程调用wait()来清理 */
    system(cmd);

    printf("After waiting for child to exit:\n");

    sleep(1);

    wait(NULL);

    system(cmd);

    /* 父进程退出，此时僵尸进程会被init进程收养并清理 */
    exit(EXIT_SUCCESS);
  }
  }
}