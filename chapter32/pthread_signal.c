#define _GNU_SOURCE
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tlpi_hdr.h"

static void *sigwaitThread(void *arg);
static void *signalSenderThread(void *arg);

// 信号处理函数
static void signalHandler(int sig, siginfo_t *info, void *ucontext) {
    printf("收到信号 %d (%s), 发送者PID: %ld, 发送值: %d\n", 
           sig, strsignal(sig), (long)info->si_pid, info->si_value.sival_int);
}

int main(int argc, char *argv[]) {
    pthread_t waiter_thread, sender_thread;
    sigset_t set;
    struct sigaction sa;
    int s;

    // 初始化信号集，添加SIGUSR1和SIGUSR2
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    
    // 在主线程中阻塞这些信号
    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0)
        errExit("pthread_sigmask");
    
    printf("主线程: 已阻塞SIGUSR1和SIGUSR2信号\n");
    
    // 设置SIGINT的信号处理函数（用于演示）
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signalHandler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction");
    
    printf("主线程: 已设置SIGINT的处理函数\n");
    
    // 创建等待信号的线程
    s = pthread_create(&waiter_thread, NULL, sigwaitThread, &set);
    if (s != 0)
        errExitEN(s, "pthread_create");
    
    // 创建发送信号的线程
    s = pthread_create(&sender_thread, NULL, signalSenderThread, NULL);
    if (s != 0)
        errExitEN(s, "pthread_create");
    
    // 等待线程结束
    s = pthread_join(sender_thread, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    
    // 向等待线程发送终止信号
    printf("主线程: 向等待线程发送SIGTERM信号\n");
    s = pthread_kill(waiter_thread, SIGTERM);
    if (s != 0)
        errExitEN(s, "pthread_kill");
    
    s = pthread_join(waiter_thread, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    
    printf("主线程: 所有线程已结束\n");
    exit(EXIT_SUCCESS);
}

// 等待信号的线程
static void *sigwaitThread(void *arg) {
    sigset_t *set = (sigset_t *)arg;
    int sig, s;
    
    printf("等待线程: 开始等待信号\n");
    
    for (;;) {
        // 使用sigwait等待信号
        s = sigwait(set, &sig);
        if (s != 0)
            errExitEN(s, "sigwait");
        
        printf("等待线程: 通过sigwait收到信号 %d (%s)\n", sig, strsignal(sig));
        
        // 如果收到SIGTERM，则退出线程
        if (sig == SIGTERM) {
            printf("等待线程: 收到终止信号，退出\n");
            break;
        }
    }
    
    return NULL;
}

// 发送信号的线程
static void *signalSenderThread(void *arg) {
    pthread_t main_thread = pthread_self();
    union sigval value;
    int s;
    
    printf("发送线程: 开始发送信号\n");
    sleep(1);  // 等待其他线程准备就绪
    
    // 使用pthread_kill发送SIGUSR1
    printf("发送线程: 使用pthread_kill发送SIGUSR1到主线程\n");
    s = pthread_kill(main_thread, SIGUSR1);
    if (s != 0)
        errExitEN(s, "pthread_kill");
    
    sleep(1);
    
    // 使用pthread_sigqueue发送带数据的SIGUSR2
    printf("发送线程: 使用pthread_sigqueue发送SIGUSR2到主线程\n");
    value.sival_int = 42;
    s = pthread_sigqueue(main_thread, SIGUSR2, value);
    if (s != 0)
        errExitEN(s, "pthread_sigqueue");
    
    sleep(1);
    
    // 发送SIGINT到进程（将被信号处理函数捕获）
    printf("发送线程: 发送SIGINT到进程\n");
    value.sival_int = 100;
    sigqueue(getpid(), SIGINT, value);
    
    sleep(1);
    printf("发送线程: 结束\n");
    return NULL;
}
