#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;
    int status;

    // 创建第一个子进程
    pid1 = fork();
    if (pid1 == 0) {
        printf("子进程1 PID: %ld\n", (long)getpid());
        sleep(3);
        exit(1);
    }

    // 创建第二个子进程
    pid2 = fork();
    if (pid2 == 0) {
        printf("子进程2 PID: %ld\n", (long)getpid());
        sleep(1);
        exit(2);
    }

    // 等待特定子进程
    printf("等待子进程2...\n");
    waitpid(pid2, &status, 0);
    printf("子进程2 已终止\n");

    // 非阻塞等待
    if (waitpid(pid1, &status, WNOHANG) == 0) {
        printf("子进程1 仍在运行\n");
    }

    // 等待子进程1
    waitpid(pid1, &status, 0);
    printf("子进程1 已终止\n");

    return 0;
}