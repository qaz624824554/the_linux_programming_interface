#include "curr_time.h"
#include "print_wait_status.h"
#include "tlpi_hdr.h"
#include <signal.h>
#include <sys/wait.h>

/* 全局变量：记录当前存活的子进程数量 */
static volatile int numLiveChildren = 0;

/* SIGCHLD信号处理函数 */
static void sigchldHandler(int sig) {
  int status, savedErrno;
  pid_t childPid;

  /* 保存errno，避免信号处理函数改变errno的值 */
  savedErrno = errno;

  printf("%s handler: Caught SIGCHLD\n", currTime("%T"));

  /* 使用循环和WNOHANG选项来收割所有已终止的子进程
   * WNOHANG表示非阻塞，如果没有子进程终止则立即返回0
   * 这样可以处理多个子进程同时终止但只收到一个SIGCHLD信号的情况 */
  while ((childPid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("%s handler: Reaped child %ld - ", currTime("%T"), (long)childPid);
    printWaitStatus(NULL, status);
    numLiveChildren--; /* 减少存活子进程计数 */
  }

  /* 检查waitpid是否出错（除了ECHILD错误，ECHILD表示没有子进程） */
  if (childPid == -1 && errno != ECHILD) {
    errExit("waitpid");
  }

  /* 模拟信号处理函数执行较长时间的情况 */
  sleep(5);
  printf("%s handler: returning\n", currTime("%T"));

  /* 恢复原来的errno值 */
  errno = savedErrno;
}

int main(int argc, char *argv[]) {
  int j, sigCnt;
  sigset_t blockMask, emptyMask;
  struct sigaction sa;

  /* 检查命令行参数 */
  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s child-sleep-time...\n", argv[0]);
  }

  /* 禁用stdout缓冲，确保输出立即显示 */
  setbuf(stdout, NULL);

  /* 初始化变量 */
  sigCnt = 0;
  numLiveChildren = argc - 1; /* 子进程数量等于命令行参数数量减1 */

  /* 设置SIGCHLD信号处理器 */
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = sigchldHandler;

  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  /* 创建信号掩码，用于阻塞SIGCHLD信号 */
  sigemptyset(&blockMask);
  sigaddset(&blockMask, SIGCHLD);

  /* 阻塞SIGCHLD信号，防止在创建子进程过程中信号处理函数被调用 */
  if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) {
    errExit("sigprocmask");
  }

  /* 根据命令行参数创建多个子进程 */
  for (j = 1; j < argc; j++) {
    switch (fork()) {
    case -1:
      errExit("fork");
    case 0:
      /* 子进程：休眠指定时间后退出 */
      sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
      printf("%s Child %d (PID=%ld) exiting\n", currTime("%T"), j,
             (long)getpid());
      _exit(EXIT_SUCCESS);
    default:
      /* 父进程：继续创建下一个子进程 */
      break;
    }
  }

  /* 创建空的信号掩码，用于sigsuspend */
  sigemptyset(&emptyMask);
  
  /* 等待所有子进程终止
   * sigsuspend会暂时解除对SIGCHLD的阻塞，等待信号到达
   * 当SIGCHLD信号到达并处理完毕后，sigsuspend返回 */
  while (numLiveChildren > 0) {
    if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
      errExit("sigsuspend");
    }
    sigCnt++; /* 记录收到SIGCHLD信号的次数 */
  }

  /* 打印统计信息 */
  printf("%s All %d children have terminated; SIGCHLD was caught %d times\n",
         currTime("%T"), argc - 1, sigCnt);
  exit(EXIT_SUCCESS);
}