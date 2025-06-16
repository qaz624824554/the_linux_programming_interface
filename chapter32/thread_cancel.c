#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "tlpi_hdr.h"

// 清理函数
static void cleanup_handler(void *arg) {
    printf("Cleanup handler called for thread %s\n", (char *)arg);
    free(arg);
}

// 测试异步取消的线程函数
static void *asyncCancelThread(void *arg) {
    char *thread_name = (char *)arg;
    int s;
    
    printf("Thread %s: Starting (async cancel)\n", thread_name);
    
    // 设置取消状态为启用，取消类型为异步
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s != 0) {
        errExit("pthread_setcancelstate");
    }
    
    s = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (s != 0) {
        errExit("pthread_setcanceltype");
    }
    
    // 注册清理函数
    pthread_cleanup_push(cleanup_handler, strdup(thread_name));
    
    // 无限循环，等待被取消
    while (1) {
        printf("Thread %s: Working... (async)\n", thread_name);
        sleep(1);
    }
    
    pthread_cleanup_pop(1);
    return NULL;
}

// 测试延迟取消的线程函数
static void *deferredCancelThread(void *arg) {
    char *thread_name = (char *)arg;
    int s, i;
    
    printf("Thread %s: Starting (deferred cancel)\n", thread_name);
    
    // 设置取消状态为启用，取消类型为延迟（默认）
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s != 0) {
        errExit("pthread_setcancelstate");
    }
    
    s = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if (s != 0) {
        errExit("pthread_setcanceltype");
    }
    
    // 注册清理函数
    pthread_cleanup_push(cleanup_handler, strdup(thread_name));
    
    // 循环工作，定期检查取消点
    for (i = 0; i < 20; i++) {
        printf("Thread %s: Working iteration %d (deferred)\n", thread_name, i);
        
        // 手动测试取消点
        pthread_testcancel();
        
        // 模拟一些工作
        usleep(500000); // 0.5秒
    }
    
    pthread_cleanup_pop(1);
    printf("Thread %s: Completed normally\n", thread_name);
    return NULL;
}

// 测试禁用取消的线程函数
static void *disabledCancelThread(void *arg) {
    char *thread_name = (char *)arg;
    int s, i;
    int old_state;
    
    printf("Thread %s: Starting (cancel disabled)\n", thread_name);
    
    // 禁用取消
    s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
    if (s != 0) {
        errExit("pthread_setcancelstate");
    }
    
    printf("Thread %s: Cancel disabled, old state was %s\n", 
           thread_name, 
           (old_state == PTHREAD_CANCEL_ENABLE) ? "ENABLE" : "DISABLE");
    
    // 注册清理函数
    pthread_cleanup_push(cleanup_handler, strdup(thread_name));
    
    // 在禁用取消状态下工作
    for (i = 0; i < 5; i++) {
        printf("Thread %s: Working with cancel disabled, iteration %d\n", thread_name, i);
        sleep(1);
    }
    
    // 重新启用取消
    printf("Thread %s: Re-enabling cancel\n", thread_name);
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s != 0) {
        errExit("pthread_setcancelstate");
    }
    
    // 继续工作，现在可以被取消
    for (i = 0; i < 10; i++) {
        printf("Thread %s: Working with cancel enabled, iteration %d\n", thread_name, i);
        pthread_testcancel(); // 检查取消点
        sleep(1);
    }
    
    pthread_cleanup_pop(1);
    printf("Thread %s: Completed normally\n", thread_name);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2, t3;
    int s;
    void *res;
    
    printf("Main thread: Starting thread cancellation demo\n");
    
    // 创建异步取消线程
    s = pthread_create(&t1, NULL, asyncCancelThread, "ASYNC");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 创建延迟取消线程
    s = pthread_create(&t2, NULL, deferredCancelThread, "DEFERRED");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 创建禁用取消线程
    s = pthread_create(&t3, NULL, disabledCancelThread, "DISABLED");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 让线程运行一段时间
    sleep(3);
    
    // 取消异步线程
    printf("Main thread: Canceling async thread\n");
    s = pthread_cancel(t1);
    if (s != 0) {
        errExit("pthread_cancel");
    }
    
    // 等待异步线程结束
    s = pthread_join(t1, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    if (res == PTHREAD_CANCELED) {
        printf("Main thread: Async thread was canceled\n");
    } else {
        printf("Main thread: Async thread terminated normally\n");
    }
    
    // 让其他线程继续运行
    sleep(2);
    
    // 取消延迟线程
    printf("Main thread: Canceling deferred thread\n");
    s = pthread_cancel(t2);
    if (s != 0) {
        errExit("pthread_cancel");
    }
    
    // 等待延迟线程结束
    s = pthread_join(t2, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    if (res == PTHREAD_CANCELED) {
        printf("Main thread: Deferred thread was canceled\n");
    } else {
        printf("Main thread: Deferred thread terminated normally\n");
    }
    
    // 让禁用取消的线程运行更长时间
    sleep(3);
    
    // 尝试取消禁用取消的线程
    printf("Main thread: Trying to cancel disabled thread\n");
    s = pthread_cancel(t3);
    if (s != 0) {
        errExit("pthread_cancel");
    }
    
    // 等待禁用取消线程结束
    s = pthread_join(t3, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    if (res == PTHREAD_CANCELED) {
        printf("Main thread: Disabled thread was canceled\n");
    } else {
        printf("Main thread: Disabled thread terminated normally\n");
    }
    
    printf("Main thread: All threads completed\n");
    
    exit(EXIT_SUCCESS);
}
