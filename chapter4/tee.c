#include "../lib/tlpi_hdr.h"
#include <fcntl.h>

/*
 * tee程序流程图:
 * 
 * 开始
 * |
 * v
 * 解析命令行参数 (-a 选项表示追加模式)
 * |
 * v
 * 计算命令行中指定的文件数量
 * |
 * v
 * 为文件描述符和文件名分配内存
 * |
 * v
 * 打开所有指定的文件
 * |
 * v
 * 循环读取标准输入
 * |   |
 * |   v
 * |   将读取的数据写入标准输出
 * |   |
 * |   v
 * |   将读取的数据写入所有打开的文件
 * |   |
 * |<--+
 * |
 * v
 * 关闭所有打开的文件
 * |
 * v
 * 释放分配的内存
 * |
 * v
 * 结束
 */
int main(int argc, char *argv[])
{
    int opt;
    int append_flag = 0;
    int num_files = 0;
    int *fd_array = NULL;
    char **file_names = NULL;
    char buffer[BUFSIZ];
    ssize_t num_read;
    int i;

    /* Parse command line options */
    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
        case 'a':
            append_flag = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-a] [file...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* Count the number of files specified on command line */
    num_files = argc - optind;

    /* Allocate arrays for file descriptors and file names */
    if (num_files > 0) {
        fd_array = malloc(num_files * sizeof(int));
        file_names = malloc(num_files * sizeof(char *));
        if (fd_array == NULL || file_names == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        /* Open each file */
        for (i = 0; i < num_files; i++) {
            file_names[i] = argv[optind + i];
            fd_array[i] = open(file_names[i], 
                               O_WRONLY | O_CREAT | (append_flag ? O_APPEND : O_TRUNC),
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
            if (fd_array[i] == -1) {
                fprintf(stderr, "Error opening file %s: %s\n", 
                        file_names[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }

    /* Read from stdin and write to stdout and all specified files */
    while ((num_read = read(STDIN_FILENO, buffer, BUFSIZ)) > 0) {
        /* Write to stdout */
        if (write(STDOUT_FILENO, buffer, num_read) != num_read) {
            fprintf(stderr, "Error writing to stdout: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Write to each file */
        for (i = 0; i < num_files; i++) {
            if (write(fd_array[i], buffer, num_read) != num_read) {
                fprintf(stderr, "Error writing to file %s: %s\n", 
                        file_names[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }

    if (num_read == -1) {
        fprintf(stderr, "Error reading from stdin: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Close all files */
    for (i = 0; i < num_files; i++) {
        if (close(fd_array[i]) == -1) {
            fprintf(stderr, "Error closing file %s: %s\n", 
                    file_names[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Free allocated memory */
    free(fd_array);
    free(file_names);

    exit(EXIT_SUCCESS);
    
}
