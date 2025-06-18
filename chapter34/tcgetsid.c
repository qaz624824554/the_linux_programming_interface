#define _GNU_SOURCE
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    int fd;
    pid_t sid;

    // 打开当前控制终端
    fd = open("/dev/tty", O_RDONLY);
    if (fd == -1)
        errExit("open");

    printf("当前进程 PID: %ld\n", (long)getpid());
    printf("当前进程 PGID: %ld\n", (long)getpgrp());
    printf("当前进程 SID (通过getsid): %ld\n", (long)getsid(0));

    // 使用 tcgetsid() 获取与终端相关联的会话ID
    sid = tcgetsid(fd);
    if (sid == -1)
        errExit("tcgetsid");

    printf("终端关联的会话ID (通过tcgetsid): %ld\n", (long)sid);

    // 检查两个会话ID是否相同
    if (sid == getsid(0))
        printf("终端的会话首进程就是当前进程的会话首进程\n");
    else
        printf("终端的会话首进程与当前进程的会话首进程不同\n");

    close(fd);
    exit(EXIT_SUCCESS);
}
