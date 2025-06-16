#include <pthread.h>
#include "tlpi_hdr.h"

// 条件变量，用于通知有线程终止
static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;
// 互斥锁，保护共享变量
static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;

// 总线程数
static int totThreads = 0;
// 存活线程数
static int numLive = 0;
// 未被join的已终止线程数
static int numUnjoined = 0;

// 线程状态枚举
enum tstate {
  TS_ALIVE,      // 存活状态
  TS_TERMINATED, // 已终止状态
  TS_JOINED      // 已被join状态
};

// 线程信息结构体
static struct {
  pthread_t tid;      // 线程ID
  enum tstate state;  // 线程状态
  int sleepTime;      // 睡眠时间
} *thread;

// 线程函数
static void *threadFunc(void *arg) {
  int idx = *((int *)arg);  // 获取线程索引
  int s;
  
  // 睡眠指定时间
  sleep(thread[idx].sleepTime);
  printf("Thread %d terminating\n", idx);
  
  // 加锁，更新共享状态
  s = pthread_mutex_lock(&threadMutex);
  if (s != 0) {
    errExit("pthread_mutex_lock");
  }
  
  numUnjoined++;  // 增加未join的线程计数
  thread[idx].state = TS_TERMINATED;  // 设置线程状态为已终止
  
  s = pthread_mutex_unlock(&threadMutex);
  if (s != 0) {
    errExit("pthread_mutex_unlock");
  }
  
  // 发送条件变量信号，通知主线程有线程终止
  s = pthread_cond_signal(&threadDied);
  if (s != 0) {
    errExit("pthread_cond_signal");
  }
  
  return NULL;
}

int main(int argc, char *argv[]) {
  int s, idx;

  // 检查命令行参数
  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s n [sleep-time[...]]]\n", argv[0]);
  }

  // 分配线程信息数组内存
  thread = calloc(argc - 1, sizeof(*thread));
  if (thread == NULL) {
    errExit("calloc");
  }

  // 创建所有线程
  for (idx = 0; idx < argc - 1; idx++) {
    thread[idx].sleepTime = getInt(argv[idx + 1], GN_NONNEG, NULL);
    thread[idx].state = TS_ALIVE;
    s = pthread_create(&thread[idx].tid, NULL, threadFunc, &idx);
    if (s != 0) {
      errExit("pthread_create");
    }
  }

  // 初始化线程计数
  totThreads = argc - 1;
  numLive = totThreads;

  // 主循环：等待并join所有线程
  while(numLive > 0) {
    // 加锁保护共享变量
    s = pthread_mutex_lock(&threadMutex);
    if (s != 0) {
      errExit("pthread_mutex_lock");
    }

    // 等待有线程终止
    while(numUnjoined == 0) {
      s = pthread_cond_wait(&threadDied, &threadMutex);
      if (s != 0) {
        errExit("pthread_cond_wait");
      }
    }

    // 遍历所有线程，join已终止的线程
    for (idx = 0; idx < totThreads; idx++) {
      if (thread[idx].state == TS_TERMINATED) {
        s = pthread_join(thread[idx].tid, NULL);
        if (s != 0) {
          errExit("pthread_join");
        }
        thread[idx].state = TS_JOINED;  // 更新状态为已join
        numLive--;      // 减少存活线程计数
        numUnjoined--;  // 减少未join线程计数
        printf("Reaped thread %d (numLive=%d)\n", idx, numLive);
      }
    }

    // 释放锁
    s = pthread_mutex_unlock(&threadMutex);
    if (s != 0) {
      errExit("pthread_mutex_unlock");
    }
  }

  exit(EXIT_SUCCESS);
}