#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SERVER_FIFO "/tmp/seqnum_server"
#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_client.%ld"
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

struct request {
    pid_t pid;
    int seqLen;
};

struct response {
    int seqNum;
};

int main(int argc, char *argv[]) {
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0;
    
    /* ① 创建服务器的众所周知的 FIFO */
    umask(0);  /* 确保 FIFO 权限正确 */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    
    /* ② 打开 FIFO 以便读取 */
    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1) {
        perror("open server FIFO");
        exit(EXIT_FAILURE);
    }
    
    /* ③ 再次打开服务器的 FIFO，这次是为了写入数据 */
    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1) {
        perror("open server FIFO for write");
        exit(EXIT_FAILURE);
    }
    
    /* ④ 忽略 SIGPIPE 信号 */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    
    printf("服务器启动，等待客户端请求...\n");
    
    /* ⑤ 进入循环从每个进入的客户端请求中读取数据并响应 */
    for (;;) {
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "错误：读取客户端请求失败或不完整\n");
            continue;
        }
        
        printf("接收到客户端请求：PID=%ld, seqLen=%d\n", (long) req.pid, req.seqLen);
        
        /* ⑥ 构建客户端 FIFO 的名称 */
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
        
        /* ⑦ 打开客户端 FIFO */
        clientFd = open(clientFifo, O_WRONLY);
        if (clientFd == -1) {
            /* ⑧ 如果打开客户端 FIFO 时发生了错误，丢弃那个客户端的请求 */
            fprintf(stderr, "无法打开客户端 FIFO %s: %s\n", clientFifo, strerror(errno));
            continue;
        }
        
        /* 准备响应 */
        resp.seqNum = seqNum;
        
        /* 发送响应给客户端 */
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
            fprintf(stderr, "错误：向客户端写入响应失败\n");
        } else {
            printf("发送序列号 %d 给客户端 PID %ld\n", resp.seqNum, (long) req.pid);
        }
        
        /* 更新序列号 */
        seqNum += req.seqLen;
        
        /* 关闭客户端 FIFO */
        if (close(clientFd) == -1) {
            perror("close client FIFO");
        }
    }
}
