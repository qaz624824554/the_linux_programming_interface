#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    pid_t pid, pgid, old_pgrp;
    int fd, status;

    // 打开控制终端
    fd = open("/dev/tty", O_RDONLY);
    if (fd == -1)
        errExit("open");

    // 获取当前前台进程组
    old_pgrp = tcgetpgrp(fd);
    if (old_pgrp == -1)
        errExit("tcgetpgrp");

    printf("父进程 PID: %ld, PGID: %ld\n", (long)getpid(), (long)getpgrp());
    printf("当前前台进程组ID: %ld\n", (long)old_pgrp);

    // 创建子进程
    switch (pid = fork()) {
    case -1:
        errExit("fork");

    case 0:     // 子进程
        // 创建新进程组
        if (setpgid(0, 0) == -1)
            errExit("setpgid");
            
        pgid = getpgrp();
        printf("子进程 PID: %ld, 新 PGID: %ld\n", (long)getpid(), (long)pgid);
        
        // 设置自己为前台进程组
        if (tcsetpgrp(fd, pgid) == -1)
            errExit("tcsetpgrp");
            
        printf("子进程: 已将自己的进程组设为前台进程组\n");
        
        // 验证
        pid_t new_fg = tcgetpgrp(fd);
        if (new_fg == -1)
            errExit("tcgetpgrp");
            
        printf("子进程: 当前前台进程组ID: %ld\n", (long)new_fg);
        
        // 暂停2秒，让父进程有时间检查前台进程组
        sleep(2);
        
        // 将前台进程组设回父进程的进程组
        printf("子进程: 将前台进程组设回父进程的进程组\n");
        if (tcsetpgrp(fd, old_pgrp) == -1)
            errExit("tcsetpgrp");
            
        _exit(EXIT_SUCCESS);

    default:    // 父进程
        // 等待子进程完成
        sleep(1);
        
        // 检查前台进程组是否已更改
        pid_t curr_fg = tcgetpgrp(fd);
        if (curr_fg == -1)
            errExit("tcgetpgrp");
            
        printf("父进程: 当前前台进程组ID: %ld\n", (long)curr_fg);
        
        if (curr_fg == pid)
            printf("父进程: 确认子进程的进程组已成为前台进程组\n");
        
        // 等待子进程结束
        if (waitpid(pid, &status, 0) == -1)
            errExit("waitpid");
            
        // 再次验证前台进程组已恢复
        curr_fg = tcgetpgrp(fd);
        if (curr_fg == -1)
            errExit("tcgetpgrp");
            
        printf("父进程: 子进程退出后的前台进程组ID: %ld\n", (long)curr_fg);
        
        if (curr_fg == old_pgrp)
            printf("父进程: 前台进程组已恢复为原始值\n");
            
        close(fd);
        exit(EXIT_SUCCESS);
    }
}
