#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    pid_t pid, pgid, new_pgid;
    
    printf("父进程 PID: %ld\n", (long)getpid());
    printf("父进程 PGID: %ld\n", (long)getpgrp());
    
    // 创建子进程
    switch (pid = fork()) {
    case -1:
        errExit("fork");
        
    case 0:     // 子进程
        // 显示子进程的初始进程组
        printf("子进程 PID: %ld, 初始 PGID: %ld\n", 
               (long)getpid(), (long)getpgrp());
        
        // 修改子进程的进程组 ID 为自己的 PID (成为进程组组长)
        if (setpgid(0, 0) == -1)
            errExit("setpgid");
            
        // 验证进程组 ID 已改变
        new_pgid = getpgrp();
        printf("子进程 PID: %ld, 新 PGID: %ld\n", 
               (long)getpid(), (long)new_pgid);
               
        // 验证子进程成为进程组组长
        if (getpid() == new_pgid)
            printf("子进程现在是进程组组长\n");
            
        sleep(2);   // 给父进程时间来设置其他组
        _exit(EXIT_SUCCESS);
        
    default:    // 父进程
        sleep(1);   // 给子进程时间先执行
        
        // 创建另一个子进程并设置其进程组为第一个子进程
        pid_t pid2;
        switch (pid2 = fork()) {
        case -1:
            errExit("fork");
            
        case 0:     // 第二个子进程
            printf("子进程2 PID: %ld, 初始 PGID: %ld\n", 
                   (long)getpid(), (long)getpgrp());
                   
            // 设置进程组 ID 为第一个子进程的 PID
            if (setpgid(0, pid) == -1)
                errExit("setpgid");
                
            printf("子进程2 PID: %ld, 新 PGID: %ld\n", 
                   (long)getpid(), (long)getpgrp());
            _exit(EXIT_SUCCESS);
            
        default:    // 父进程继续
            // 等待两个子进程结束
            waitpid(pid, NULL, 0);
            waitpid(pid2, NULL, 0);
            
            printf("父进程: 两个子进程都已退出\n");
            exit(EXIT_SUCCESS);
        }
    }
}
