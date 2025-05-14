#include "../lib/tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>

#define BUF_SIZE 4096

/*
 * cp程序流程图:
 * 
 * 开始
 * |
 * v
 * 检查命令行参数 (需要源文件和目标文件)
 * |
 * v
 * 打开源文件
 * |
 * v
 * 获取源文件的权限信息
 * |
 * v
 * 创建目标文件 (使用源文件的权限)
 * |
 * v
 * 循环读取源文件内容
 * |   |
 * |   v
 * |   检查读取的块是否全为零 (可能是文件空洞)
 * |   |
 * |   v
 * |   如果是空洞，在目标文件中跳过这部分
 * |   |
 * |   v
 * |   如果不是空洞，将数据写入目标文件
 * |   |
 * |<--+
 * |
 * v
 * 关闭源文件和目标文件
 * |
 * v
 * 结束
 */
int main(int argc, char *argv[])
{
    int inputFd, outputFd;
    ssize_t numRead;
    char buf[BUF_SIZE];
    struct stat statbuf;
    off_t lastOffset, currOffset;

    if (argc != 3)
        usageErr("%s source-file dest-file\n", argv[0]);

    /* Open input file */
    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("open %s", argv[1]);

    /* Get input file permissions to use for output file */
    if (fstat(inputFd, &statbuf) == -1)
        errExit("fstat");


    /* Open output file */
    outputFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
                    statbuf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
    if (outputFd == -1)
        errExit("open %s", argv[2]);

    /* Copy data, preserving holes */
    lastOffset = 0;

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        /* Check if the block contains only zeros (a potential hole) */
        int i;
        int allZeros = 1;
        
        for (i = 0; i < numRead; i++) {
            if (buf[i] != '\0') {
                allZeros = 0;
                break;
            }
        }

        /* Get current offset in input file */
        currOffset = lseek(inputFd, 0, SEEK_CUR);
        if (currOffset == -1)
            errExit("lseek");

        if (allZeros) {
            /* Skip writing this block of zeros and seek forward in output file */
            if (lseek(outputFd, currOffset, SEEK_SET) == -1)
                errExit("lseek");
        } else {
            /* Write actual data */
            if (write(outputFd, buf, numRead) != numRead)
                fatal("write() failed");
        }

        lastOffset = currOffset;
    }

    if (numRead == -1)
        errExit("read");

    if (close(inputFd) == -1)
        errExit("close input");
    if (close(outputFd) == -1)
        errExit("close output");

    exit(EXIT_SUCCESS);
}
