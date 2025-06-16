#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tlpi_hdr.h"

// 定义线程局部存储变量
__thread int tls_var = 0;
__thread char tls_buffer[256];

// 普通全局变量用于对比
static int global_var = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// 线程函数
static void *threadFunc(void *arg) {
    char *thread_name = (char *)arg;
    int i;
    
    printf("Thread %s started\n", thread_name);
    
    // 初始化线程局部存储变量
    tls_var = 100;
    snprintf(tls_buffer, sizeof(tls_buffer), "Thread %s local data", thread_name);
    
    printf("Thread %s: Initial TLS var = %d\n", thread_name, tls_var);
    printf("Thread %s: TLS buffer = %s\n", thread_name, tls_buffer);
    
    // 修改线程局部存储变量
    for (i = 0; i < 5; i++) {
        tls_var += 10;
        printf("Thread %s: TLS var = %d (iteration %d)\n", thread_name, tls_var, i + 1);
        
        // 同时修改全局变量（需要加锁）
        pthread_mutex_lock(&mtx);
        global_var++;
        printf("Thread %s: Global var = %d\n", thread_name, global_var);
        pthread_mutex_unlock(&mtx);
        
        sleep(1);
    }
    
    // 显示最终的线程局部存储值
    printf("Thread %s: Final TLS var = %d\n", thread_name, tls_var);
    printf("Thread %s: Final TLS buffer = %s\n", thread_name, tls_buffer);
    
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2, t3;
    int s;
    
    printf("Main thread started\n");
    
    // 主线程中的线程局部存储变量
    tls_var = 999;
    snprintf(tls_buffer, sizeof(tls_buffer), "Main thread local data");
    
    printf("Main thread: TLS var = %d\n", tls_var);
    printf("Main thread: TLS buffer = %s\n", tls_buffer);
    
    // 创建多个线程
    s = pthread_create(&t1, NULL, threadFunc, "A");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    s = pthread_create(&t2, NULL, threadFunc, "B");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    s = pthread_create(&t3, NULL, threadFunc, "C");
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 主线程继续修改自己的TLS变量
    sleep(2);
    tls_var += 50;
    printf("Main thread: Modified TLS var = %d\n", tls_var);
    
    // 等待所有线程完成
    s = pthread_join(t1, NULL);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    s = pthread_join(t2, NULL);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    s = pthread_join(t3, NULL);
    if (s != 0) {
        errExit("pthread_join");
    }
    
    // 显示主线程最终的TLS值
    printf("Main thread: Final TLS var = %d\n", tls_var);
    printf("Main thread: Final TLS buffer = %s\n", tls_buffer);
    printf("Final global var = %d\n", global_var);
    
    printf("All threads completed\n");
    
    exit(EXIT_SUCCESS);
}
