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

// 测试pthread_testcancel的线程函数
static void *testCancelThread(void *arg) {
    char *thread_name = (char *)arg;
    int s, i;
    
    printf("Thread %s: Starting\n", thread_name);
    
    // 设置取消状态为启用，取消类型为延迟
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
    
    // 循环工作，在不同位置调用pthread_testcancel
    for (i = 0; i < 20; i++) {
        printf("Thread %s: Working iteration %d\n", thread_name, i);
        
        // 模拟一些计算工作（没有取消点）
        volatile int sum = 0;
        for (int j = 0; j < 1000000; j++) {
            sum += j;
        }
        
        // 在循环中间手动检查取消点
        if (i % 3 == 0) {
            printf("Thread %s: Checking for cancellation at iteration %d\n", thread_name, i);
            pthread_testcancel(); // 手动取消点
        }
        
        // 模拟更多工作
        usleep(200000); // 0.2秒
        
        // 在循环末尾再次检查取消点
        if (i % 5 == 0) {
            printf("Thread %s: Another cancellation check at iteration %d\n", thread_name, i);
            pthread_testcancel(); // 另一个手动取消点
        }
    }
    
    pthread_cleanup_pop(1);
    printf("Thread %s: Completed normally\n", thread_name);
    return NULL;
}

// 不使用pthread_testcancel的线程函数（对比用）
static void *noTestCancelThread(void *arg) {
    char *thread_name = (char *)arg;
    int s, i;
    
    printf("Thread %s: Starting (no testcancel)\n", thread_name);
    
    // 设置取消状态为启用，取消类型为延迟
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
    
    // 循环工作，但不调用pthread_testcancel
    for (i = 0; i < 20; i++) {
        printf("Thread %s: Working iteration %d (no testcancel)\n", thread_name, i);
        
        // 模拟一些计算工作（没有取消点）
        volatile int sum = 0;
        for (int j = 0; j < 1000000; j++) {
            sum += j;
        }
        
        // 注意：这里不调用pthread_testcancel()
        // 只有在遇到系统调用等自然取消点时才会被取消
        
        usleep(200000); // 0.2秒 - 这是一个取消点
    }
    
    pthread_cleanup_pop(1);
    printf("Thread %s: Completed normally\n", thread_name);
    return NULL;
}

// 在关键区域禁用取消的线程函数
static void *criticalSectionThread(void *arg) {
    char *thread_name = (char *)arg;
    int s, i;
    int old_state;
    
    printf("Thread %s: Starting (critical section demo)\n", thread_name);
    
    // 注册清理函数
    pthread_cleanup_push(cleanup_handler, strdup(thread_name));
    
    for (i = 0; i < 10; i++) {
        printf("Thread %s: Before critical section %d\n", thread_name, i);
        
        // 进入关键区域前检查取消
        pthread_testcancel();
        
        // 进入关键区域 - 禁用取消
        printf("Thread %s: Entering critical section %d\n", thread_name, i);
        s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        if (s != 0) {
            errExit("pthread_setcancelstate");
        }
        
        // 在关键区域中工作
        printf("Thread %s: Working in critical section %d\n", thread_name, i);
        sleep(1);
        
        // 离开关键区域 - 恢复取消状态
        printf("Thread %s: Leaving critical section %d\n", thread_name, i);
        s = pthread_setcancelstate(old_state, NULL);
        if (s != 0) {
            errExit("pthread_setcancelstate");
        }
        
        // 离开关键区域后立即检查取消
        printf("Thread %s: Checking cancellation after critical section %d\n", thread_name, i);
        pthread_testcancel();
        
        usleep(300000); // 0.3秒
    }
    
    pthread_cleanup_pop(1);
    printf("Thread %s: Completed normally\n", thread_name);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2, t3;
    int s;
    void *res;
    
    printf("Main thread: Starting pthread_testcancel demo\n");
    printf("==============================================\n");
    
    // 创建使用pthread_testcancel的线程
    s = pthread_create(&t1, NULL, testCancelThread, "TESTCANCEL");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 创建不使用pthread_testcancel的线程
    s = pthread_create(&t2, NULL, noTestCancelThread, "NOTESTCANCEL");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 创建关键区域演示线程
    s = pthread_create(&t3, NULL, criticalSectionThread, "CRITICAL");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 让线程运行一段时间
    sleep(2);
    
    printf("\nMain thread: Canceling all threads\n");
    printf("===================================\n");
    
    // 取消第一个线程（使用testcancel）
    printf("Main thread: Canceling testcancel thread\n");
    s = pthread_cancel(t1);
    if (s != 0) {
        errExit("pthread_cancel");
    }
    
    // 取消第二个线程（不使用testcancel）
    printf("Main thread: Canceling no-testcancel thread\n");
    s = pthread_cancel(t2);
    if (s != 0) {
        errExit("pthread_cancel");
    }
    
    // 取消第三个线程（关键区域）
    printf("Main thread: Canceling critical section thread\n");
    s = pthread_cancel(t3);
    if (s != 0) {
        errExit("pthread_cancel");
    }
    
    printf("\nMain thread: Waiting for threads to complete\n");
    printf("=============================================\n");
    
    // 等待第一个线程结束
    s = pthread_join(t1, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    if (res == PTHREAD_CANCELED) {
        printf("Main thread: TESTCANCEL thread was canceled\n");
    } else {
        printf("Main thread: TESTCANCEL thread terminated normally\n");
    }
    
    // 等待第二个线程结束
    s = pthread_join(t2, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    if (res == PTHREAD_CANCELED) {
        printf("Main thread: NOTESTCANCEL thread was canceled\n");
    } else {
        printf("Main thread: NOTESTCANCEL thread terminated normally\n");
    }
    
    // 等待第三个线程结束
    s = pthread_join(t3, &res);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    if (res == PTHREAD_CANCELED) {
        printf("Main thread: CRITICAL thread was canceled\n");
    } else {
        printf("Main thread: CRITICAL thread terminated normally\n");
    }
    
    printf("\nMain thread: All threads completed\n");
    printf("pthread_testcancel() allows threads to check for cancellation\n");
    printf("at specific points, providing more control over when cancellation\n");
    printf("can occur in deferred cancellation mode.\n");
    
    exit(EXIT_SUCCESS);
}
