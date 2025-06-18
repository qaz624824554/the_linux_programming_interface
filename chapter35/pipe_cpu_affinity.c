/*
 * pipe_cpu_affinity.c
 *
 * 演示在相同CPU与不同CPU上运行的进程之间
 * 通过管道通信的性能差异。
 * 
 * 这个实验结果说明在管道通信场景中，进程间并行性带来的好处超过了CPU缓存局部性的优势。
 */

#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <sched.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE (1024 * 1024)  /* 1MB缓冲区 */
#define TRANSFER_COUNT 100         /* 传输次数 */
#define PIPE_BUF_SIZE (1024 * 1024) /* 1MB管道缓冲区 */

/* I/O阻塞时使用的延迟（纳秒） */
#define IO_DELAY_NS 1000

static void set_cpu_affinity(int cpu_id) {
    cpu_set_t mask;
    
    CPU_ZERO(&mask);
    CPU_SET(cpu_id, &mask);
    
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1)
        errExit("sched_setaffinity");
}

static long get_time_us(void) {
    struct timespec ts;
    
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
        errExit("clock_gettime");
    
    return (ts.tv_sec * 1000000) + (ts.tv_nsec / 1000);
}

static void transfer_data(int fd_in, int fd_out, size_t buffer_size, int count) {
    char *buffer;
    int i, bytes_read, bytes_written, total_read, total_written;
    long start_time, end_time;
    struct timespec delay;
    
    /* 设置I/O阻塞时的延迟 */
    delay.tv_sec = 0;
    delay.tv_nsec = IO_DELAY_NS;
    
    /* 分配缓冲区 */
    buffer = malloc(buffer_size);
    if (buffer == NULL)
        errExit("malloc");
    
    /* 用数据填充缓冲区 */
    for (i = 0; i < buffer_size; i++)
        buffer[i] = i % 256;
    
    start_time = get_time_us();
    
    /* 通过管道传输数据'count'次 */
    for (i = 0; i < count; i++) {
        /* 将缓冲区写入管道，处理部分写入 */
        total_written = 0;
        while (total_written < buffer_size) {
            bytes_written = write(fd_out, buffer + total_written, buffer_size - total_written);
            if (bytes_written == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    /* 管道已满，短暂让出CPU */
                    nanosleep(&delay, NULL);
                    continue;
                }
                errExit("write");
            }
            total_written += bytes_written;
        }
        
        /* 从管道读取数据到缓冲区，处理部分读取 */
        total_read = 0;
        while (total_read < buffer_size) {
            bytes_read = read(fd_in, buffer + total_read, buffer_size - total_read);
            if (bytes_read == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    /* 没有可用数据，短暂让出CPU */
                    nanosleep(&delay, NULL);
                    continue;
                }
                errExit("read");
            }
            total_read += bytes_read;
        }
    }
    
    end_time = get_time_us();
    
    long elapsed_us = end_time - start_time;
    double elapsed_s = elapsed_us / 1000000.0;
    double total_data_mb = ((double)buffer_size * count * 2) / (1024 * 1024);
    double throughput = total_data_mb / elapsed_s;
    
    printf("\n---------------------------------------------------------\n");
    printf("性能结果：\n");
    printf("---------------------------------------------------------\n");
    printf("总传输数据：%.2f MB\n", total_data_mb);
    printf("耗时：%.3f 秒 (%.2f μs)\n", elapsed_s, (double)elapsed_us);
    printf("吞吐量：%.2f MB/s\n", throughput);
    printf("平均往返延迟：%.2f μs\n", (double)elapsed_us / count);
    printf("---------------------------------------------------------\n");
    
    free(buffer);
}

int main(int argc, char *argv[]) {
    int pipefd1[2], pipefd2[2];
    pid_t child_pid;
    int num_cpus, cpu1, cpu2;
    Boolean same_cpu;
    
    /* 解析命令行 */
    if (argc != 2 || (strcmp(argv[1], "same") != 0 && strcmp(argv[1], "diff") != 0))
        usageErr("%s same|diff\n"
                "same - 在同一CPU上运行两个进程\n"
                "diff - 在不同CPU上运行进程\n", argv[0]);
    
    same_cpu = strcmp(argv[1], "same") == 0;
    
    /* 获取CPU数量 */
    num_cpus = sysconf(_SC_NPROCESSORS_CONF);
    if (num_cpus == -1)
        errExit("sysconf");
    
    if (num_cpus < 2 && !same_cpu) {
        fprintf(stderr, "该系统只有%d个CPU。无法在不同CPU上运行。\n", num_cpus);
        exit(EXIT_FAILURE);
    }
    
    printf("==========================================================\n");
    printf("管道通信性能测试\n");
    printf("==========================================================\n");
    printf("此测试测量两个进程在相同CPU与不同CPU上运行时\n");
    printf("通过管道通信的性能差异。当进程在同一CPU上运行时，它们可以\n");
    printf("受益于管道数据的CPU缓存局部性。\n\n");
    
    printf("测试配置：\n");
    printf("- 缓冲区大小：%d 字节（%d KB）\n", BUFFER_SIZE, BUFFER_SIZE/1024);
    printf("- 传输次数：%d 次往返\n", TRANSFER_COUNT);
    printf("- 总数据量：%d MB\n", (BUFFER_SIZE * TRANSFER_COUNT * 2) / (1024*1024));
    printf("- 管道缓冲区大小：%d KB\n", PIPE_BUF_SIZE/1024);
    printf("- CPU配置：%s\n", same_cpu ? "相同CPU" : "不同CPU");
    printf("- 可用CPU数量：%d\n\n", num_cpus);
    
    /* 创建管道 */
    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1)
        errExit("pipe");
    
    /* 增加管道缓冲区大小以提高性能 */
    if (fcntl(pipefd1[0], F_SETPIPE_SZ, PIPE_BUF_SIZE) == -1)
        errExit("fcntl F_SETPIPE_SZ");
    if (fcntl(pipefd1[1], F_SETPIPE_SZ, PIPE_BUF_SIZE) == -1)
        errExit("fcntl F_SETPIPE_SZ");
    if (fcntl(pipefd2[0], F_SETPIPE_SZ, PIPE_BUF_SIZE) == -1)
        errExit("fcntl F_SETPIPE_SZ");
    if (fcntl(pipefd2[1], F_SETPIPE_SZ, PIPE_BUF_SIZE) == -1)
        errExit("fcntl F_SETPIPE_SZ");
    
    /* 设置管道为非阻塞模式以提高性能 */
    fcntl(pipefd1[0], F_SETFL, O_NONBLOCK);
    fcntl(pipefd1[1], F_SETFL, O_NONBLOCK);
    fcntl(pipefd2[0], F_SETFL, O_NONBLOCK);
    fcntl(pipefd2[1], F_SETFL, O_NONBLOCK);
    
    /* 选择要使用的CPU */
    cpu1 = 0;
    cpu2 = same_cpu ? 0 : 10; /* 如果是不同CPU，则使用CPU 0和1 */
    
    printf("开始测试...\n");
    printf("父进程将在CPU %d上运行\n", cpu1);
    printf("子进程将在CPU %d上运行\n", cpu2);
    
    /* 创建子进程 */
    child_pid = fork();
    if (child_pid == -1)
        errExit("fork");
    
    if (child_pid == 0) {
        /* 子进程 */
        
        /* 关闭未使用的管道端 */
        close(pipefd1[1]);
        close(pipefd2[0]);
        
        /* 设置CPU亲和性 */
        set_cpu_affinity(cpu2);
        
        printf("子进程在CPU %d上运行\n", cpu2);
        
        /* 子进程从pipe1读取并写入pipe2 */
        transfer_data(pipefd1[0], pipefd2[1], BUFFER_SIZE, TRANSFER_COUNT);
        
        /* 关闭剩余的管道端 */
        close(pipefd1[0]);
        close(pipefd2[1]);
        
        exit(EXIT_SUCCESS);
    }
    else {
        /* 父进程 */
        
        /* 关闭未使用的管道端 */
        close(pipefd1[0]);
        close(pipefd2[1]);
        
        /* 设置CPU亲和性 */
        set_cpu_affinity(cpu1);
        
        printf("父进程在CPU %d上运行\n", cpu1);
        
        /* 父进程写入pipe1并从pipe2读取 */
        transfer_data(pipefd2[0], pipefd1[1], BUFFER_SIZE, TRANSFER_COUNT);
        
        /* 等待子进程退出 */
        wait(NULL);
        
        /* 关闭剩余的管道端 */
        close(pipefd1[1]);
        close(pipefd2[0]);
    }
    
    printf("\n测试完成。\n");
    printf("==========================================================\n");
    if (same_cpu) {
        printf("当进程在同一CPU上运行时，它们受益于\n");
        printf("管道数据的CPU缓存局部性。管道缓冲区可以\n");
        printf("保留在CPU的缓存中，减少内存访问延迟。\n");
    } else {
        printf("当进程在不同CPU上运行时，管道数据必须\n");
        printf("在CPU缓存之间传输，这可能会增加延迟\n");
        printf("相比于单CPU通信。\n");
    }
    printf("==========================================================\n");
    
    exit(EXIT_SUCCESS);
} 