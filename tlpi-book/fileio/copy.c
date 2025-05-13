/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 4-1 */
/* 包含必要的头文件 */
#include <sys/stat.h>   /* 提供文件状态和权限相关的定义 */
#include <fcntl.h>      /* 提供文件控制选项，如O_RDONLY, O_WRONLY等 */
#include "tlpi_hdr.h"   /* 包含自定义的头文件，提供错误处理函数等 */

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024   /* 定义缓冲区大小为1024字节 */
#endif

int
main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags;   /* 文件描述符和打开标志 */
    mode_t filePerms;                   /* 文件权限 */
    ssize_t numRead;                    /* 读取的字节数 */
    char buf[BUF_SIZE];                 /* 数据缓冲区 */

    /* 检查命令行参数，必须提供源文件和目标文件 */
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s old-file new-file\n", argv[0]);

    /* 打开输入文件（只读模式） */
    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)  /* 打开失败则报错退出 */
        errExit("opening file %s", argv[1]);

    /* 设置输出文件的打开标志和权限 */
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;  /* 创建、只写、截断 */
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- 所有用户可读写 */
    /* 打开输出文件 */
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1)  /* 打开失败则报错退出 */
        errExit("opening file %s", argv[2]);

    /* 数据传输循环：从输入文件读取数据并写入输出文件 */

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)  /* 读取数据到缓冲区 */
        if (write(outputFd, buf, numRead) != numRead)     /* 将数据写入输出文件 */
            fatal("write() returned error or partial write occurred");  /* 写入错误或不完整 */
    if (numRead == -1)  /* 读取过程中发生错误 */
        errExit("read");

    /* 关闭文件 */
    if (close(inputFd) == -1)  /* 关闭输入文件 */
        errExit("close input");
    if (close(outputFd) == -1)  /* 关闭输出文件 */
        errExit("close output");

    exit(EXIT_SUCCESS);  /* 成功退出程序 */
}
