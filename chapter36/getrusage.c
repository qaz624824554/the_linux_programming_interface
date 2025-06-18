#include "tlpi_hdr.h"
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>

static void print_rusage(const char *leader, const struct rusage *ru) {
    printf("%s:\n", leader);
    printf("  用户CPU时间: %ld.%06ld 秒\n", 
           (long) ru->ru_utime.tv_sec, (long) ru->ru_utime.tv_usec);
    printf("  系统CPU时间: %ld.%06ld 秒\n", 
           (long) ru->ru_stime.tv_sec, (long) ru->ru_stime.tv_usec);
    printf("  最大常驻集大小: %ld KB\n", ru->ru_maxrss);
    printf("  页面错误: %ld (不需I/O) %ld (需I/O)\n", 
           ru->ru_minflt, ru->ru_majflt);
    printf("  块I/O操作: %ld 输入, %ld 输出\n", 
           ru->ru_inblock, ru->ru_oublock);
    printf("  等待次数: %ld 自愿, %ld 非自愿\n", 
           ru->ru_nvcsw, ru->ru_nivcsw);
}

/* 执行一些CPU密集型计算作为示例 */
static void do_some_work(void) {
    const int size = 10000000;
    double *array;
    int i;

    array = malloc(size * sizeof(double));
    if (array == NULL)
        errExit("malloc");

    /* 执行一些CPU和内存操作 */
    for (i = 0; i < size; i++)
        array[i] = i * 3.14159265;

    for (i = 0; i < size; i++)
        array[i] = sqrt(array[i]);

    free(array);
}

int main(int argc, char *argv[]) {
    struct rusage ru, ru_child;
    pid_t child_pid;
    int status;

    printf("演示getrusage()系统调用\n\n");

    /* 首先获取父进程的资源使用情况基准 */
    if (getrusage(RUSAGE_SELF, &ru) == -1)
        errExit("getrusage");
    printf("初始资源使用情况:\n");
    print_rusage("RUSAGE_SELF", &ru);

    printf("\n创建子进程执行一些CPU密集型工作...\n\n");

    /* 创建子进程 */
    child_pid = fork();
    if (child_pid == -1)
        errExit("fork");

    if (child_pid == 0) {
        /* 在子进程中执行一些工作 */
        do_some_work();
        exit(EXIT_SUCCESS);
    }

    /* 父进程等待子进程完成 */
    if (wait(&status) == -1)
        errExit("wait");

    printf("子进程已退出，状态 = %d\n\n", WEXITSTATUS(status));

    /* 获取子进程的资源使用情况 */
    if (getrusage(RUSAGE_CHILDREN, &ru_child) == -1)
        errExit("getrusage");
    print_rusage("RUSAGE_CHILDREN", &ru_child);

    /* 父进程执行一些工作 */
    printf("\n父进程现在执行一些CPU密集型工作...\n\n");
    do_some_work();

    /* 再次获取父进程的资源使用情况 */
    if (getrusage(RUSAGE_SELF, &ru) == -1)
        errExit("getrusage");
    print_rusage("RUSAGE_SELF (更新后)", &ru);

    exit(EXIT_SUCCESS);
}
