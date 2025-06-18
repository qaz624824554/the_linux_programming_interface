#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    pid_t pid, pgid, ppid;
    
    pid = getpid();
    pgid = getpgrp();  // 获取当前进程的进程组ID
    ppid = getppid();  // 获取父进程ID
    
    printf("PID: %ld\n", (long)pid);
    printf("进程组ID (PGID): %ld\n", (long)pgid);
    printf("父进程ID (PPID): %ld\n", (long)ppid);
    
    // 另一种获取进程组ID的方式，getpgrp()等同于getpgid(0)
    pid_t pgid2 = getpgid(0);
    printf("通过getpgid(0)获取的PGID: %ld\n", (long)pgid2);
    
    // 获取指定进程的进程组ID
    pid_t parent_pgid = getpgid(ppid);
    printf("父进程的进程组ID: %ld\n", (long)parent_pgid);
    
    // 检查当前进程是否为进程组组长
    if (pid == pgid) {
        printf("当前进程是进程组组长\n");
    } else {
        printf("当前进程不是进程组组长\n");
    }
    
    exit(EXIT_SUCCESS);
}
