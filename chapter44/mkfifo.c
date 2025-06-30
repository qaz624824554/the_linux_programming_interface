#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#define FIFO_NAME "myfifo"
#define BUFFER_SIZE 1024

/* 信号处理函数，用于清理 FIFO */
static void cleanup_handler(int sig) {
    printf("\n接收到信号 %d，清理 FIFO 文件...\n", sig);
    unlink(FIFO_NAME);
    exit(EXIT_SUCCESS);
}

/* 演示基本的 FIFO 创建和通信 */
static void demonstrateBasicFifo(void) {
    pid_t pid;
    int fd;
    char write_buffer[] = "Hello from parent process via FIFO!";
    char read_buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    printf("=== 演示基本 FIFO 操作 ===\n");
    
    /* 创建 FIFO */
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo 失败");
            return;
        }
        printf("FIFO 已存在，继续使用\n");
    } else {
        printf("成功创建 FIFO: %s\n", FIFO_NAME);
    }
    
    /* 创建子进程 */
    pid = fork();
    if (pid == -1) {
        perror("fork 失败");
        unlink(FIFO_NAME);
        return;
    }
    
    if (pid == 0) {
        /* 子进程 - 读取者 */
        printf("子进程：准备从 FIFO 读取数据...\n");
        
        /* 以读模式打开 FIFO */
        fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) {
            perror("子进程：打开 FIFO 失败");
            _exit(EXIT_FAILURE);
        }
        
        printf("子进程：成功打开 FIFO 进行读取\n");
        
        /* 从 FIFO 读取数据 */
        bytes_read = read(fd, read_buffer, BUFFER_SIZE - 1);
        if (bytes_read > 0) {
            read_buffer[bytes_read] = '\0';
            printf("子进程：从 FIFO 读取到: \"%s\"\n", read_buffer);
        } else {
            printf("子进程：读取失败或没有数据\n");
        }
        
        close(fd);
        printf("子进程：完成读取并退出\n");
        _exit(EXIT_SUCCESS);
        
    } else {
        /* 父进程 - 写入者 */
        sleep(1);  /* 确保子进程先打开 FIFO */
        
        printf("父进程：准备向 FIFO 写入数据...\n");
        
        /* 以写模式打开 FIFO */
        fd = open(FIFO_NAME, O_WRONLY);
        if (fd == -1) {
            perror("父进程：打开 FIFO 失败");
            wait(NULL);
            unlink(FIFO_NAME);
            return;
        }
        
        printf("父进程：成功打开 FIFO 进行写入\n");
        
        /* 向 FIFO 写入数据 */
        if (write(fd, write_buffer, strlen(write_buffer)) == -1) {
            perror("父进程：写入 FIFO 失败");
        } else {
            printf("父进程：向 FIFO 写入: \"%s\"\n", write_buffer);
        }
        
        close(fd);
        
        /* 等待子进程完成 */
        wait(NULL);
        printf("父进程：子进程已完成\n");
    }
    
    printf("\n");
}

/* 演示多个写入者和一个读取者 */
static void demonstrateMultipleWriters(void) {
    pid_t pids[3];
    int i, status;
    int fd;
    char read_buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    printf("=== 演示多个写入者和一个读取者 ===\n");
    
    /* 确保 FIFO 存在 */
    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo 失败");
        return;
    }
    
    /* 创建读取者进程 */
    pids[0] = fork();
    if (pids[0] == -1) {
        perror("fork 读取者失败");
        return;
    }
    
    if (pids[0] == 0) {
        /* 读取者子进程 */
        printf("读取者：准备从 FIFO 读取数据...\n");
        
        fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) {
            perror("读取者：打开 FIFO 失败");
            _exit(EXIT_FAILURE);
        }
        
        /* 持续读取数据直到没有更多写入者 */
        int message_count = 0;
        while ((bytes_read = read(fd, read_buffer, BUFFER_SIZE - 1)) > 0) {
            read_buffer[bytes_read] = '\0';
            printf("读取者：收到消息 %d: \"%s\"\n", ++message_count, read_buffer);
        }
        
        close(fd);
        printf("读取者：完成读取，共收到 %d 条消息\n", message_count);
        _exit(EXIT_SUCCESS);
    }
    
    /* 创建两个写入者进程 */
    for (i = 1; i <= 2; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork 写入者失败");
            continue;
        }
        
        if (pids[i] == 0) {
            /* 写入者子进程 */
            char message[100];
            sleep(i);  /* 不同的延迟 */
            
            snprintf(message, sizeof(message), "来自写入者 %d (PID: %d) 的消息", i, getpid());
            
            printf("写入者 %d：准备写入消息...\n", i);
            fd = open(FIFO_NAME, O_WRONLY);
            if (fd == -1) {
                perror("写入者：打开 FIFO 失败");
                _exit(EXIT_FAILURE);
            }
            
            if (write(fd, message, strlen(message)) == -1) {
                perror("写入者：写入失败");
            } else {
                printf("写入者 %d：成功写入消息\n", i);
            }
            
            close(fd);
            _exit(EXIT_SUCCESS);
        }
    }
    
    /* 父进程等待所有子进程完成 */
    for (i = 1; i <= 2; i++) {
        waitpid(pids[i], &status, 0);
        printf("写入者 %d 已完成\n", i);
    }
    
    /* 等待读取者完成 */
    waitpid(pids[0], &status, 0);
    printf("读取者已完成\n");
    
    printf("\n");
}

/* 演示非阻塞 FIFO 操作 */
static void demonstrateNonBlockingFifo(void) {
    int fd_read, fd_write;
    char message[] = "非阻塞消息";
    char buffer[BUFFER_SIZE];
    ssize_t bytes;
    
    printf("=== 演示非阻塞 FIFO 操作 ===\n");
    
    /* 确保 FIFO 存在 */
    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo 失败");
        return;
    }
    
    /* 以非阻塞模式打开 FIFO 进行写入 */
    printf("以非阻塞模式打开 FIFO 进行写入...\n");
    fd_write = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
    if (fd_write == -1) {
        if (errno == ENXIO) {
            printf("没有读取者，写入操作会阻塞（在非阻塞模式下返回 ENXIO）\n");
        } else {
            perror("打开 FIFO 写入失败");
        }
        return;
    }
    
    /* 以非阻塞模式打开 FIFO 进行读取 */
    printf("以非阻塞模式打开 FIFO 进行读取...\n");
    fd_read = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
    if (fd_read == -1) {
        perror("打开 FIFO 读取失败");
        close(fd_write);
        return;
    }
    
    /* 写入数据 */
    printf("写入数据到 FIFO...\n");
    if (write(fd_write, message, strlen(message)) == -1) {
        perror("非阻塞写入失败");
    } else {
        printf("成功写入: \"%s\"\n", message);
    }
    
    /* 立即尝试读取 */
    printf("立即尝试从 FIFO 读取...\n");
    bytes = read(fd_read, buffer, BUFFER_SIZE - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("成功读取: \"%s\"\n", buffer);
    } else if (bytes == 0) {
        printf("没有数据可读取（EOF）\n");
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("没有数据可读取（非阻塞模式下正常）\n");
        } else {
            perror("读取失败");
        }
    }
    
    close(fd_read);
    close(fd_write);
    
    printf("\n");
}

/* 演示 FIFO 作为服务器-客户端通信 */
static void demonstrateClientServer(void) {
    pid_t server_pid, client_pid;
    int server_fd, client_fd;
    char server_message[] = "服务器响应: 收到客户端请求";
    char client_message[] = "客户端请求: 请求服务器处理";
    char buffer[BUFFER_SIZE];
    ssize_t bytes;
    
    printf("=== 演示 FIFO 服务器-客户端通信 ===\n");
    
    /* 确保 FIFO 存在 */
    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo 失败");
        return;
    }
    
    /* 创建服务器进程 */
    server_pid = fork();
    if (server_pid == -1) {
        perror("fork 服务器失败");
        return;
    }
    
    if (server_pid == 0) {
        /* 服务器进程 */
        printf("服务器：启动并等待客户端连接...\n");
        
        /* 服务器首先打开读取端 */
        server_fd = open(FIFO_NAME, O_RDONLY);
        if (server_fd == -1) {
            perror("服务器：打开 FIFO 失败");
            _exit(EXIT_FAILURE);
        }
        
        printf("服务器：已准备好接收客户端请求\n");
        
        /* 读取客户端请求 */
        bytes = read(server_fd, buffer, BUFFER_SIZE - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("服务器：收到客户端请求: \"%s\"\n", buffer);
        }
        
        close(server_fd);
        
        /* 服务器现在作为写入者响应 */
        server_fd = open(FIFO_NAME, O_WRONLY);
        if (server_fd == -1) {
            perror("服务器：重新打开 FIFO 失败");
            _exit(EXIT_FAILURE);
        }
        
        /* 发送响应给客户端 */
        if (write(server_fd, server_message, strlen(server_message)) == -1) {
            perror("服务器：发送响应失败");
        } else {
            printf("服务器：发送响应给客户端\n");
        }
        
        close(server_fd);
        printf("服务器：完成服务并退出\n");
        _exit(EXIT_SUCCESS);
    }
    
    /* 稍等确保服务器先启动 */
    sleep(1);
    
    /* 创建客户端进程 */
    client_pid = fork();
    if (client_pid == -1) {
        perror("fork 客户端失败");
        waitpid(server_pid, NULL, 0);
        return;
    }
    
    if (client_pid == 0) {
        /* 客户端进程 */
        printf("客户端：连接到服务器...\n");
        
        /* 客户端作为写入者发送请求 */
        client_fd = open(FIFO_NAME, O_WRONLY);
        if (client_fd == -1) {
            perror("客户端：打开 FIFO 失败");
            _exit(EXIT_FAILURE);
        }
        
        printf("客户端：发送请求到服务器\n");
        if (write(client_fd, client_message, strlen(client_message)) == -1) {
            perror("客户端：发送请求失败");
        }
        
        close(client_fd);
        
        /* 等待服务器处理，然后读取响应 */
        sleep(1);
        
        client_fd = open(FIFO_NAME, O_RDONLY);
        if (client_fd == -1) {
            perror("客户端：重新打开 FIFO 失败");
            _exit(EXIT_FAILURE);
        }
        
        /* 读取服务器响应 */
        bytes = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("客户端：收到服务器响应: \"%s\"\n", buffer);
        }
        
        close(client_fd);
        printf("客户端：完成通信并退出\n");
        _exit(EXIT_SUCCESS);
    }
    
    /* 父进程等待服务器和客户端完成 */
    waitpid(server_pid, NULL, 0);
    waitpid(client_pid, NULL, 0);
    
    printf("服务器-客户端通信演示完成\n");
    printf("\n");
}

int main(void) {
    /* 设置信号处理器用于清理 */
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    
    printf("=== mkfifo() 和命名管道 (FIFO) 功能演示 ===\n");
    printf("命名管道（FIFO）是一种特殊的文件，可用于不相关进程间的通信\n\n");
    
    /* 执行各种演示 */
    demonstrateBasicFifo();
    demonstrateMultipleWriters();
    demonstrateNonBlockingFifo();
    demonstrateClientServer();
    
    /* 清理 FIFO 文件 */
    if (unlink(FIFO_NAME) == -1) {
        perror("删除 FIFO 文件失败");
    } else {
        printf("已清理 FIFO 文件: %s\n", FIFO_NAME);
    }
    
    printf("\n=== mkfifo() 演示完成 ===\n");
    printf("\n主要特点:\n");
    printf("1. mkfifo() 创建命名管道，可在文件系统中看到\n");
    printf("2. FIFO 遵循先进先出原则，支持多个读写者\n");
    printf("3. 可用于不相关进程间的通信\n");
    printf("4. 支持阻塞和非阻塞模式\n");
    printf("5. 可实现复杂的进程间通信模式\n");
    
    return 0;
}
