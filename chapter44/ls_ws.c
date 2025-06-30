#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t pid1, pid2;
    int status;
    
    printf("=== 演示 ls | wc -l 管道操作 ===\n");
    
    /* 创建管道 */
    if (pipe(pipefd) == -1) {
        perror("pipe 创建失败");
        exit(EXIT_FAILURE);
    }
    
    /* 创建第一个子进程执行 ls */
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    }
    
    if (pid1 == 0) {
        /* 子进程1: 执行 ls */
        close(pipefd[0]);  /* 关闭读端 */
        
        /* 将标准输出重定向到管道写端 */
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2 失败");
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]);  /* 关闭原来的管道写端 */
        
        /* 执行 ls 命令 */
        execlp("ls", "ls", NULL);
        perror("execlp ls 失败");
        exit(EXIT_FAILURE);
    }
    
    /* 创建第二个子进程执行 wc -l */
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    }
    
    if (pid2 == 0) {
        /* 子进程2: 执行 wc -l */
        close(pipefd[1]);  /* 关闭写端 */
        
        /* 将标准输入重定向到管道读端 */
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2 失败");
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]);  /* 关闭原来的管道读端 */
        
        /* 执行 wc -l 命令 */
        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc 失败");
        exit(EXIT_FAILURE);
    }
    
    /* 父进程: 关闭管道两端 */
    close(pipefd[0]);
    close(pipefd[1]);
    
    /* 等待两个子进程完成 */
    printf("等待 ls 进程完成...\n");
    waitpid(pid1, &status, 0);
    if (WIFEXITED(status)) {
        printf("ls 进程正常退出，退出码: %d\n", WEXITSTATUS(status));
    } else {
        printf("ls 进程异常退出\n");
    }
    
    printf("等待 wc 进程完成...\n");
    waitpid(pid2, &status, 0);
    if (WIFEXITED(status)) {
        printf("wc 进程正常退出，退出码: %d\n", WEXITSTATUS(status));
    } else {
        printf("wc 进程异常退出\n");
    }
    
    printf("管道操作演示完成\n");
    
    return 0;
}
