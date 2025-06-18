#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/wait.h>

static void display_rlimit(const char *msg, int resource) {
    struct rlimit rlim;

    if (getrlimit(resource, &rlim) == -1)
        errExit("getrlimit %s", msg);

    printf("%s: ", msg);
    if (rlim.rlim_cur == RLIM_INFINITY)
        printf("soft=unlimited; ");
    else
        printf("soft=%lld; ", (long long) rlim.rlim_cur);

    if (rlim.rlim_max == RLIM_INFINITY)
        printf("hard=unlimited\n");
    else
        printf("hard=%lld\n", (long long) rlim.rlim_max);
}

int main(int argc, char *argv[]) {
    struct rlimit rlim;
    int j;
    pid_t childPid;

    printf("当前资源限制:\n");
    display_rlimit("RLIMIT_CPU (CPU时间)", RLIMIT_CPU);
    display_rlimit("RLIMIT_FSIZE (文件大小)", RLIMIT_FSIZE);
    display_rlimit("RLIMIT_DATA (数据段大小)", RLIMIT_DATA);
    display_rlimit("RLIMIT_STACK (栈大小)", RLIMIT_STACK);
    display_rlimit("RLIMIT_CORE (核心转储文件大小)", RLIMIT_CORE);
    display_rlimit("RLIMIT_NOFILE (文件描述符数量)", RLIMIT_NOFILE);
    display_rlimit("RLIMIT_AS (虚拟内存大小)", RLIMIT_AS);

    /* 修改RLIMIT_NOFILE限制 */
    printf("\n修改RLIMIT_NOFILE限制\n");
    if (getrlimit(RLIMIT_NOFILE, &rlim) == -1)
        errExit("getrlimit");

    /* 保存原始值以便后续恢复 */
    struct rlimit orig_rlim = rlim;

    /* 将软限制设置为硬限制的一半 */
    rlim.rlim_cur = rlim.rlim_max / 2;
    printf("将RLIMIT_NOFILE软限制设置为: %lld\n", (long long) rlim.rlim_cur);

    if (setrlimit(RLIMIT_NOFILE, &rlim) == -1)
        errExit("setrlimit");

    display_rlimit("新的RLIMIT_NOFILE限制", RLIMIT_NOFILE);

    /* 测试限制：尝试打开超过软限制数量的文件 */
    printf("\n尝试打开%lld个文件 (超过软限制)\n", (long long) rlim.rlim_cur + 10);
    
    int *fds = calloc(rlim.rlim_cur + 10, sizeof(int));
    if (fds == NULL)
        errExit("calloc");

    /* 尝试打开文件直到失败 */
    for (j = 0; j < rlim.rlim_cur + 10; j++) {
        fds[j] = open("/dev/null", O_RDONLY);
        if (fds[j] == -1) {
            printf("打开文件失败，文件数量: %d: %s\n", j, strerror(errno));
            break;
        }
    }

    printf("成功打开的文件数量: %d\n", j);

    /* 关闭所有打开的文件 */
    for (j = 0; j < rlim.rlim_cur + 10; j++) {
        if (fds[j] != -1)
            close(fds[j]);
    }
    free(fds);

    /* 恢复原始限制 */
    if (setrlimit(RLIMIT_NOFILE, &orig_rlim) == -1)
        errExit("setrlimit - restore");

    printf("\n恢复原始RLIMIT_NOFILE限制\n");
    display_rlimit("恢复后的RLIMIT_NOFILE限制", RLIMIT_NOFILE);

    /* 测试RLIMIT_CPU限制 */
    printf("\n测试RLIMIT_CPU限制 (在子进程中)\n");
    
    /* 设置CPU时间限制为1秒 */
    rlim.rlim_cur = 1;  /* 1秒软限制 */
    rlim.rlim_max = 2;  /* 2秒硬限制 */
    
    childPid = fork();
    if (childPid == -1)
        errExit("fork");

    if (childPid == 0) {  /* 子进程 */
        printf("子进程: 设置CPU时间限制为1秒\n");
        
        if (setrlimit(RLIMIT_CPU, &rlim) == -1)
            errExit("setrlimit-RLIMIT_CPU");
            
        display_rlimit("子进程RLIMIT_CPU", RLIMIT_CPU);
        
        printf("子进程: 开始无限循环...\n");
        for (;;)  /* 消耗CPU时间的无限循环 */
            continue;
            
        exit(EXIT_SUCCESS);  /* 不应该到达这里 */
    }

    /* 父进程等待子进程终止 */
    int status;
    if (waitpid(childPid, &status, 0) == -1)
        errExit("waitpid");

    printf("子进程已终止");
    if (WIFSIGNALED(status))
        printf(" - 被信号 %s 终止\n", strsignal(WTERMSIG(status)));
    else
        printf(" - 状态=%d\n", WEXITSTATUS(status));

    exit(EXIT_SUCCESS);
}
