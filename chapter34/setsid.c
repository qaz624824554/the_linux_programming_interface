#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    pid_t pid, sid, pgid;
    
    printf("父进程 PID: %ld\n", (long)getpid());
    printf("父进程 PGID: %ld\n", (long)getpgrp());
    printf("父进程 SID: %ld\n", (long)getsid(0));
    
    switch (pid = fork()) {
    case -1:
        errExit("fork");
        
    case 0:     // 子进程
        // 显示子进程的初始进程组和会话ID
        printf("子进程 PID: %ld, 初始 PGID: %ld, 初始 SID: %ld\n", 
               (long)getpid(), (long)getpgrp(), (long)getsid(0));
        
        // 创建新会话
        sid = setsid();
        if (sid == -1)
            errExit("setsid");
            
        // 验证会话ID和进程组ID已改变
        pgid = getpgrp();
        printf("子进程 PID: %ld, 新 PGID: %ld, 新 SID: %ld\n", 
               (long)getpid(), (long)pgid, (long)sid);
               
        // 验证子进程成为会话首进程和进程组组长
        if (getpid() == sid && getpid() == pgid)
            printf("子进程现在是会话首进程和进程组组长\n");
        
        // 尝试改变控制终端
        printf("尝试打开/dev/tty: ");
        int fd = open("/dev/tty", O_RDWR);
        if (fd == -1)
            printf("失败 (预期行为，新会话没有控制终端)\n");
        else {
            printf("成功 (意外行为)\n");
            close(fd);
        }
            
        _exit(EXIT_SUCCESS);
        
    default:    // 父进程
        sleep(1);   // 给子进程时间执行
        printf("父进程: 子进程 (PID=%ld) 已创建新会话\n", (long)pid);
        exit(EXIT_SUCCESS);
    }
}
