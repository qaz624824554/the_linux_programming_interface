/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* 演示 syslog 函数的使用 */

#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "tlpi_hdr.h"

static void usageError(const char *progName) {
    fprintf(stderr, "Usage: %s [-p] [-e] [-l level] [-m mask] "
           "[-i ident] [-f facility] msg...\n", progName);
    fprintf(stderr, "    -p   在消息中包含PID\n");
    fprintf(stderr, "    -e   将消息记录到stderr\n");
    fprintf(stderr, "    -l   设置优先级 (默认: LOG_INFO)\n");
    fprintf(stderr, "    -m   设置掩码 (默认: 全部)\n");
    fprintf(stderr, "    -i   设置syslog标识 (默认: 程序名)\n");
    fprintf(stderr, "    -f   设置设施 (默认: LOG_USER)\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int options, level, mask, facility;
    const char *ident;
    int opt;
    
    /* 默认值 */
    options = 0;
    level = LOG_INFO;
    mask = LOG_UPTO(LOG_DEBUG);
    facility = LOG_USER;
    ident = NULL;
    
    /* 处理命令行参数 */
    while ((opt = getopt(argc, argv, "pel:m:i:f:")) != -1) {
        switch (opt) {
        case 'p':
            options |= LOG_PID;
            break;
        case 'e':
            options |= LOG_PERROR;
            break;
        case 'l':
            level = atoi(optarg);
            if (level < 0 || level > LOG_DEBUG)
                usageError(argv[0]);
            break;
        case 'm':
            mask = atoi(optarg);
            break;
        case 'i':
            ident = optarg;
            break;
        case 'f':
            facility = atoi(optarg);
            break;
        default:
            usageError(argv[0]);
        }
    }
    
    /* 至少需要一个消息参数 */
    if (optind >= argc)
        usageError(argv[0]);
    
    /* 打开系统日志连接 */
    openlog(ident, options, facility);
    printf("打开系统日志连接，标识：%s，选项：%d，设施：%d\n",
           (ident != NULL) ? ident : argv[0], options, facility);
    
    /* 设置日志掩码 */
    int oldMask = setlogmask(mask);
    printf("设置日志掩码为 %d (旧掩码: %d)\n", mask, oldMask);
    
    /* 构建消息 */
    char message[1024] = "";
    for (int j = optind; j < argc; j++) {
        if (j > optind)
            strcat(message, " ");
        strcat(message, argv[j]);
    }
    
    /* 发送日志消息 */
    printf("发送日志消息，级别：%d，消息：\"%s\"\n", level, message);
    syslog(level, "%s", message);
    
    /* 测试不同级别的日志 */
    syslog(LOG_EMERG, "紧急消息测试 (EMERG)");
    syslog(LOG_ALERT, "警报消息测试 (ALERT)");
    syslog(LOG_CRIT, "严重错误消息测试 (CRIT)");
    syslog(LOG_ERR, "错误消息测试 (ERR)");
    syslog(LOG_WARNING, "警告消息测试 (WARNING)");
    syslog(LOG_NOTICE, "通知消息测试 (NOTICE)");
    syslog(LOG_INFO, "信息消息测试 (INFO)");
    syslog(LOG_DEBUG, "调试消息测试 (DEBUG)");
    
    /* 关闭系统日志连接 */
    printf("关闭系统日志连接\n");
    closelog();
    
    exit(EXIT_SUCCESS);
}
