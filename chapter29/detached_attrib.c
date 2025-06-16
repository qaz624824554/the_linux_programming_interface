#include <pthread.h>
#include <unistd.h>
#include "tlpi_hdr.h"

static void *threadFunc(void *arg) {
    int thread_num = *((int *)arg);
    
    printf("Detached thread %d: Starting\n", thread_num);
    
    // 模拟一些工作
    for (int i = 1; i <= 5; i++) {
        printf("Detached thread %d: Working iteration %d\n", thread_num, i);
        sleep(1);
    }
    
    printf("Detached thread %d: Finished\n", thread_num);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2;
    pthread_attr_t attr;
    int thread1_id = 1, thread2_id = 2;
    int s;
    
    printf("Main thread: Initializing thread attributes\n");
    
    // 初始化线程属性对象
    s = pthread_attr_init(&attr);
    if (s != 0) {
        errExit("pthread_attr_init");
    }
    
    // 设置线程为分离状态
    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (s != 0) {
        errExit("pthread_attr_setdetachstate");
    }
    
    printf("Main thread: Creating detached threads\n");
    
    // 使用分离属性创建第一个线程
    s = pthread_create(&t1, &attr, threadFunc, &thread1_id);
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 使用分离属性创建第二个线程
    s = pthread_create(&t2, &attr, threadFunc, &thread2_id);
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 销毁属性对象
    s = pthread_attr_destroy(&attr);
    if (s != 0) {
        errExit("pthread_attr_destroy");
    }
    
    printf("Main thread: Detached threads created\n");
    printf("Main thread: Cannot join detached threads, sleeping to let them finish\n");
    
    // 由于线程是分离的，我们不能使用pthread_join()等待它们
    // 主线程需要等待足够长的时间让分离的线程完成工作
    sleep(7);
    
    printf("Main thread: Exiting\n");
    exit(EXIT_SUCCESS);
}
