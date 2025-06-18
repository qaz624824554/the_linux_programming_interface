#define _GNU_SOURCE
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "tlpi_hdr.h"

static void display_rusage(const char *header, const struct rusage *ru) {
    printf("%s:\n", header);
    printf("  用户CPU时间:      %ld.%06ld 秒\n", 
           (long)ru->ru_utime.tv_sec, (long)ru->ru_utime.tv_usec);
    printf("  系统CPU时间:      %ld.%06ld 秒\n", 
           (long)ru->ru_stime.tv_sec, (long)ru->ru_stime.tv_usec);
    printf("  最大常驻集大小:   %ld KB\n", ru->ru_maxrss);
    printf("  页面错误:         %ld (不需I/O) %ld (需I/O)\n", 
           ru->ru_minflt, ru->ru_majflt);
    printf("  块I/O操作:        %ld 输入, %ld 输出\n", 
           ru->ru_inblock, ru->ru_oublock);
    printf("  上下文切换:       %ld 自愿, %ld 非自愿\n", 
           ru->ru_nvcsw, ru->ru_nivcsw);
}

// 让子进程执行一些CPU密集型操作
static void consume_cpu_time(void) {
    const int SIZE = 90000000;
    double *array = malloc(SIZE * sizeof(double));
    if (array == NULL)
        errExit("malloc");
    
    printf("子进程: 开始执行CPU密集型操作...\n");
    
    // 执行一些数学运算消耗CPU时间
    for (int i = 0; i < SIZE; i++) {
        array[i] = sqrt(i * 3.14159265);
    }
    
    for (int i = 0; i < SIZE; i++) {
        array[i] = log(array[i] + 1);
    }
    
    printf("子进程: CPU密集型操作完成\n");
    free(array);
}

int main(int argc, char *argv[]) {
    struct rusage ru_before, ru_after;
    pid_t childPid;
    int status;
    
    // 获取wait()调用之前的资源使用情况
    if (getrusage(RUSAGE_CHILDREN, &ru_before) == -1)
        errExit("getrusage");
    
    printf("父进程: 创建子进程前的资源使用情况\n");
    display_rusage("RUSAGE_CHILDREN (wait前)", &ru_before);
    
    // 创建子进程
    childPid = fork();
    if (childPid == -1)
        errExit("fork");
    
    if (childPid == 0) {
        // 子进程
        consume_cpu_time();
        exit(EXIT_SUCCESS);
    }
    
    // 父进程等待子进程结束
    printf("\n父进程: 等待子进程 (PID=%ld) 结束...\n", (long)childPid);
    if (wait(&status) == -1)
        errExit("wait");
    
    if (WIFEXITED(status))
        printf("父进程: 子进程正常退出，状态=%d\n\n", WEXITSTATUS(status));
    else
        printf("父进程: 子进程异常退出\n\n");
    
    // 获取wait()调用之后的资源使用情况
    if (getrusage(RUSAGE_CHILDREN, &ru_after) == -1)
        errExit("getrusage");
    
    printf("父进程: wait()调用后的资源使用情况\n");
    display_rusage("RUSAGE_CHILDREN (wait后)", &ru_after);
    
    exit(EXIT_SUCCESS);
}
