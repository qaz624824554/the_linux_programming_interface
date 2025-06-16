#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tlpi_hdr.h"

static pthread_key_t strerrorKey;

// 线程特有数据的析构函数
static void destructor(void *buf) {
    free(buf);
}

// 初始化线程特有数据键
static void createKey(void) {
    int s;
    
    s = pthread_key_create(&strerrorKey, destructor);
    if (s != 0) {
        errExit("pthread_key_create");
    }
}

// 线程安全的strerror实现
static char *strerror_tsd(int err) {
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    int s;
    char *buf;
    
    // 确保键只被创建一次
    s = pthread_once(&once, createKey);
    if (s != 0) {
        errExit("pthread_once");
    }
    
    // 获取当前线程的特有数据
    buf = pthread_getspecific(strerrorKey);
    if (buf == NULL) {
        // 第一次调用，分配缓冲区
        buf = malloc(1000);
        if (buf == NULL) {
            errExit("malloc");
        }
        
        // 将缓冲区与键关联
        s = pthread_setspecific(strerrorKey, buf);
        if (s != 0) {
            errExit("pthread_setspecific");
        }
    }
    
    // 使用系统的strerror_r填充缓冲区
    if (strerror_r(err, buf, 1000) != 0) {
        snprintf(buf, 1000, "Unknown error %d", err);
    }
    
    return buf;
}

// 线程函数
static void *threadFunc(void *arg) {
    char *str;
    
    printf("Thread %s started\n", (char *)arg);
    
    // 测试不同的错误码
    str = strerror_tsd(EPERM);
    printf("Thread %s: EPERM = %s\n", (char *)arg, str);
    
    str = strerror_tsd(ENOENT);
    printf("Thread %s: ENOENT = %s\n", (char *)arg, str);
    
    str = strerror_tsd(ESRCH);
    printf("Thread %s: ESRCH = %s\n", (char *)arg, str);
    
    // 再次调用，验证使用的是同一个缓冲区
    str = strerror_tsd(EINTR);
    printf("Thread %s: EINTR = %s\n", (char *)arg, str);
    
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2, t3;
    int s;
    
    printf("Main thread started\n");
    
    // 主线程也测试一下
    char *str = strerror_tsd(EACCES);
    printf("Main thread: EACCES = %s\n", str);
    
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
    
    // 主线程再次测试
    str = strerror_tsd(EAGAIN);
    printf("Main thread: EAGAIN = %s\n", str);
    
    printf("All threads completed\n");
    
    exit(EXIT_SUCCESS);
}
