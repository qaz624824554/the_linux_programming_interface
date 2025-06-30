#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pipefd[2];
    pid_t cpid;
    char buf;
    
    if (argc != 2) {
        fprintf(stderr, "用法: %s <字符串>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    printf("=== 演示简单管道操作 ===\n");
    printf("将要写入管道的字符串: \"%s\"\n", argv[1]);
    
    /* ① 创建管道 */
    if (pipe(pipefd) == -1) {
        perror("pipe 创建失败");
        exit(EXIT_FAILURE);
    }
    
    /* ② 创建子进程 */
    cpid = fork();
    if (cpid == -1) {
        perror("fork 失败");
        exit(EXIT_FAILURE);
    }
    
    if (cpid == 0) {    /* 子进程 */
        /* ③ 子进程关闭管道的写入端 */
        close(pipefd[1]);
        
        printf("\n子进程开始从管道读取数据:\n");
        
        /* ④⑤⑥⑦ 从管道读取数据并写到标准输出，直到遇到文件结束 */
        while (read(pipefd[0], &buf, 1) > 0) {
            write(STDOUT_FILENO, &buf, 1);
        }
        
        /* 写入结尾换行字符 */
        write(STDOUT_FILENO, "\n", 1);
        
        /* 关闭管道的读取端 */
        close(pipefd[0]);
        
        printf("子进程读取完成并终止\n");
        _exit(EXIT_SUCCESS);
        
    } else {           /* 父进程 */
        /* ⑧ 父进程关闭管道的读取端 */
        close(pipefd[0]);
        
        printf("父进程开始写入数据到管道...\n");
        
        /* ⑨ 将命令行参数字符串写入管道 */
        write(pipefd[1], argv[1], strlen(argv[1]));
        
        /* ⑩ 关闭管道的写入端 */
        close(pipefd[1]);
        
        printf("父进程写入完成，等待子进程终止...\n");
        
        /* ⑪ 等待子进程终止 */
        wait(NULL);
        
        printf("父进程：子进程已终止\n");
    }
    
    return 0;
}
