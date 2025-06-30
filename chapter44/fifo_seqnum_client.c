#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
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

static char clientFifo[CLIENT_FIFO_NAME_LEN];

/* ① 退出处理器，确保删除客户端 FIFO */
static void removeFifo(void) {
    unlink(clientFifo);
}

int main(int argc, char *argv[]) {
    int serverFd, clientFd;
    struct request req;
    struct response resp;
    int seqLen;
    
    /* 获取序列号长度（默认为 1） */
    seqLen = (argc > 1) ? atoi(argv[1]) : 1;
    
    /* 构造客户端 FIFO 名称 */
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
    
    /* ② 创建客户端 FIFO 用于接收服务器响应 */
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR) == -1 && errno != EEXIST) {
        perror("mkfifo client FIFO");
        exit(EXIT_FAILURE);
    }
    
    /* ③ 注册退出处理器来清理客户端 FIFO */
    if (atexit(removeFifo) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }
    
    /* ④ 构建发送给服务器的请求消息 */
    req.pid = getpid();
    req.seqLen = seqLen;
    
    printf("客户端 PID %ld: 请求 %d 个序列号\n", (long) getpid(), seqLen);
    
    /* ⑤ 打开服务器 FIFO 并发送请求 */
    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1) {
        perror("open server FIFO");
        exit(EXIT_FAILURE);
    }
    
    /* ⑥ 发送请求给服务器 */
    if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
        perror("写入服务器 FIFO");
        exit(EXIT_FAILURE);
    }
    
    if (close(serverFd) == -1) {
        perror("close server FIFO");
        exit(EXIT_FAILURE);
    }
    
    /* ⑦ 打开客户端 FIFO 等待服务器响应 */
    clientFd = open(clientFifo, O_RDONLY);
    if (clientFd == -1) {
        perror("open client FIFO");
        exit(EXIT_FAILURE);
    }
    
    /* ⑧ 读取并打印服务器响应 */
    if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
        perror("读取服务器响应");
        exit(EXIT_FAILURE);
    }
    
    printf("客户端 PID %ld: 收到序列号 %d\n", (long) getpid(), resp.seqNum);
    
    /* 关闭客户端 FIFO */
    if (close(clientFd) == -1) {
        perror("close client FIFO");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
