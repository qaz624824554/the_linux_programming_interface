#include <pthread.h>
#include <unistd.h>
#include "tlpi_hdr.h"

static void *threadFunc(void *arg) {
    int thread_num = *((int *)arg);
    
    // 子线程持续运行，每秒打印一次消息
    for (int i = 1; i <= 10; i++) {
        printf("Thread %d: Running iteration %d\n", thread_num, i);
        sleep(1);
    }
    
    printf("Thread %d: Finished after 10 iterations\n", thread_num);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2;
    int thread1_id = 1, thread2_id = 2;
    int s;
    
    printf("Main thread: Creating threads\n");
    
    // 创建第一个线程
    s = pthread_create(&t1, NULL, threadFunc, &thread1_id);
    if (s != 0) {
        errExit("pthread_create");
    }
    
    // 创建第二个线程
    s = pthread_create(&t2, NULL, threadFunc, &thread2_id);
    if (s != 0) {
        errExit("pthread_create");
    }
    
    printf("Main thread: Threads created, main thread will exit now\n");
    printf("Main thread: Child threads will continue running...\n");
    
    // 主线程调用pthread_exit()退出，但不会终止整个进程
    // 子线程会继续运行直到完成
    pthread_exit(NULL);
    
    // 这行代码永远不会执行，因为主线程已经通过pthread_exit()退出
    printf("This line will never be printed\n");
}
