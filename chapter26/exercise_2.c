#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    pid_t grandparent_pid, parent_pid, child_pid;
    
    printf("祖父进程 PID: %d\n", getpid());
    grandparent_pid = getpid();
    
    // 创建父进程
    parent_pid = fork();
    if (parent_pid == -1) {
        perror("fork failed");
        exit(1);
    }
    
    if (parent_pid == 0) {
        // 父进程代码
        printf("父进程 PID: %d, PPID: %d\n", getpid(), getppid());
        
        // 创建子进程（孙进程）
        child_pid = fork();
        if (child_pid == -1) {
            perror("fork failed");
            exit(1);
        }
        
        if (child_pid == 0) {
            // 孙进程代码
            printf("孙进程 PID: %d, 初始 PPID: %d\n", getpid(), getppid());
            
            // 等待一段时间，让父进程先退出
            sleep(2);
            printf("孙进程：父进程退出后，当前 PPID: %d\n", getppid());
            
            // 继续等待，直到祖父进程调用wait()
            sleep(5);
            printf("孙进程：祖父进程wait()后，当前 PPID: %d\n", getppid());
            
            exit(0);
        } else {
            // 父进程：创建孙进程后立即退出
            printf("父进程：创建孙进程后即将退出\n");
            sleep(1);  // 确保孙进程有时间打印初始状态
            exit(0);   // 父进程退出，变成僵尸进程
        }
    } else {
        // 祖父进程代码
        printf("祖父进程：创建了父进程 %d\n", parent_pid);
        
        // 等待一段时间，让父进程和孙进程都启动
        sleep(3);
        printf("祖父进程：父进程应该已经退出，现在检查其状态\n");
        
        // 检查父进程是否变成僵尸进程
        char cmd[100];
        snprintf(cmd, sizeof(cmd), "ps -o pid,ppid,stat,comm -p %d", parent_pid);
        printf("父进程状态：\n");
        system(cmd);
        
        // 继续等待，不立即调用wait()
        printf("祖父进程：等待4秒后再调用wait()...\n");
        sleep(4);
        
        // 现在调用wait()清理僵尸进程
        int status;
        pid_t waited_pid = wait(&status);
        printf("祖父进程：wait()返回，清理了进程 %d\n", waited_pid);
        
        // 再等待一下，让孙进程打印最终状态
        sleep(2);
        
        printf("程序结束\n");
    }
    
    return 0;
}