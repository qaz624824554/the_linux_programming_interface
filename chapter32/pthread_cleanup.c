#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tlpi_hdr.h"

// 清理函数1
static void cleanup_handler1(void *arg) {
    printf("Cleanup handler 1 called with arg: %s\n", (char *)arg);
    free(arg);
}

// 清理函数2
static void cleanup_handler2(void *arg) {
    printf("Cleanup handler 2 called with arg: %s\n", (char *)arg);
    free(arg);
}

// 正常退出的线程函数
static void *normalExitThread(void *arg) {
    char *thread_name = (char *)arg;
    
    printf("Thread %s: Starting\n", thread_name);
    
    // 注册清理函数（后进先出顺序）
    pthread_cleanup_push(cleanup_handler1, strdup("Resource 1"));
    pthread_cleanup_push(cleanup_handler2, strdup("Resource 2"));
    
    // 模拟一些工作
    printf("Thread %s: Working...\n", thread_name);
    sleep(2);
    
    printf("Thread %s: Finishing normally\n", thread_name);
    
    // 正常退出时，pop(1)会执行清理函数
    pthread_cleanup_pop(1);  // 执行cleanup_handler2
    pthread_cleanup_pop(1);  // 执行cleanup_handler1
    
    return NULL;
}

// 被取消的线程函数
static void *canceledThread(void *arg) {
    char *thread_name = (char *)arg;
    
    printf("Thread %s: Starting\n", thread_name);
    
    // 注册清理函数
    pthread_cleanup_push(cleanup_handler1, strdup("Resource A"));
    pthread_cleanup_push(cleanup_handler2, strdup("Resource B"));
    
    // 设置为可取消状态
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    
    // 无限循环，等待被取消
    while (1) {
        printf("Thread %s: Working... (waiting for cancellation)\n", thread_name);
        sleep(1);
        pthread_testcancel(); // 取消点
    }
    
    // 这里不会被执行，因为线程会被取消
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    
    return NULL;
}

// 不执行清理函数的线程
static void *noCleanupThread(void *arg) {
    char *thread_name = (char *)arg;
    
    printf("Thread %s: Starting\n", thread_name);
    
    // 注册清理函数
    pthread_cleanup_push(cleanup_handler1, strdup("Resource X"));
    pthread_cleanup_push(cleanup_handler2, strdup("Resource Y"));
    
    // 模拟一些工作
    printf("Thread %s: Working...\n", thread_name);
    sleep(2);
    
    printf("Thread %s: Finishing without cleanup\n", thread_name);
    
    // pop(0)不会执行清理函数，但会移除它们
    pthread_cleanup_pop(0);  // 不执行cleanup_handler2
    pthread_cleanup_pop(0);  // 不执行cleanup_handler1
    
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2, t3;
    int s;
    void *res;
    
    printf("Main thread: Starting pthread cleanup demo\n");
    printf("==========================================\n");
    
    // 创建正常退出的线程
    printf("\n1. Testing normal exit with cleanup:\n");
    s = pthread_create(&t1, NULL, normalExitThread, "NORMAL");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    s = pthread_join(t1, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    // 创建被取消的线程
    printf("\n2. Testing thread cancellation with cleanup:\n");
    s = pthread_create(&t2, NULL, canceledThread, "CANCELED");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 让线程运行一段时间后取消它
    sleep(3);
    printf("Main thread: Canceling thread\n");
    s = pthread_cancel(t2);
    if (s != 0) {
        errExit("pthread_cancel");
    }
    
    s = pthread_join(t2, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    if (res == PTHREAD_CANCELED) {
        printf("Thread was canceled (cleanup handlers were called)\n");
    }
    
    // 创建不执行清理函数的线程
    printf("\n3. Testing exit without cleanup execution:\n");
    s = pthread_create(&t3, NULL, noCleanupThread, "NOCLEANUP");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    s = pthread_join(t3, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    printf("\nMain thread: All threads completed\n");
    printf("Cleanup handlers are called when:\n");
    printf("- Thread is canceled\n");
    printf("- pthread_cleanup_pop(1) is called\n");
    printf("- pthread_exit() is called\n");
    
    exit(EXIT_SUCCESS);
}
