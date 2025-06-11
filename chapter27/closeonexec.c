#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int flags;

  /* 如果提供了命令行参数，则设置 close-on-exec 标志 */
  if (argc > 1) {
    /* 获取标准输出的文件描述符标志 */
    flags = fcntl(STDOUT_FILENO, F_GETFD);
    if (flags == -1) {
      errExit("fcntl - F_GETFD");
    }

    /* 设置 FD_CLOEXEC 标志，使得在 exec 时自动关闭该文件描述符 */
    flags |= FD_CLOEXEC;

    /* 将修改后的标志应用到标准输出文件描述符 */
    if (fcntl(STDOUT_FILENO, F_SETFD, flags) == -1) {
      errExit("fcntl - F_SETFD");
    }
  }

  /* 执行 ls 命令列出当前程序文件的详细信息
   * 如果之前设置了 FD_CLOEXEC，标准输出将在 exec 时被关闭
   * 否则标准输出保持打开状态 */
  execlp("ls", "ls", "-l", argv[0], (char *)NULL);
  
  /* 如果 execlp 返回，说明执行失败 */
  errExit("execlp");
}