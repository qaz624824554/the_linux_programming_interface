/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* 演示 getlogin() 和 getlogin_r() 函数的使用 */

#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <utmpx.h>
#include "tlpi_hdr.h"

static void demonstrateGetlogin(void) {
    char *loginName;
    
    printf("=== 使用 getlogin() 获取登录名 ===\n");
    
    loginName = getlogin();
    if (loginName == NULL) {
        if (errno == 0) {
            printf("getlogin(): 无法确定登录名 (可能不是从终端登录)\n");
        } else {
            errMsg("getlogin");
        }
    } else {
        printf("登录名: %s\n", loginName);
    }
    printf("\n");
}

static void demonstrateGetloginR(void) {
    char buf[256];
    int result;
    
    printf("=== 使用 getlogin_r() 获取登录名 ===\n");
    
    result = getlogin_r(buf, sizeof(buf));
    if (result != 0) {
        errno = result;
        if (result == ENOTTY) {
            printf("getlogin_r(): 标准输入不是终端\n");
        } else if (result == ENXIO) {
            printf("getlogin_r(): 无法确定控制终端\n");
        } else if (result == ERANGE) {
            printf("getlogin_r(): 缓冲区太小\n");
        } else {
            errMsg("getlogin_r");
        }
    } else {
        printf("登录名: %s\n", buf);
    }
    printf("\n");
}

static void demonstrateSmallBuffer(void) {
    char smallBuf[4];  /* 故意使用很小的缓冲区 */
    int result;
    
    printf("=== 测试 getlogin_r() 使用小缓冲区 ===\n");
    printf("缓冲区大小: %zu 字节\n", sizeof(smallBuf));
    
    result = getlogin_r(smallBuf, sizeof(smallBuf));
    if (result != 0) {
        errno = result;
        if (result == ERANGE) {
            printf("预期错误: 缓冲区太小 (ERANGE)\n");
        } else {
            errMsg("getlogin_r (small buffer)");
        }
    } else {
        printf("意外成功: %s\n", smallBuf);
    }
    printf("\n");
}

static void compareWithOtherMethods(void) {
    char *loginName;
    char *envUser;
    struct passwd *pwd;
    uid_t uid;
    
    printf("=== 比较不同的用户名获取方法 ===\n");
    
    /* getlogin() */
    loginName = getlogin();
    printf("getlogin():     %s\n", 
           (loginName != NULL) ? loginName : "NULL");
    
    /* 环境变量 USER */
    envUser = getenv("USER");
    printf("$USER:          %s\n", 
           (envUser != NULL) ? envUser : "未设置");
    
    /* 环境变量 LOGNAME */
    envUser = getenv("LOGNAME");
    printf("$LOGNAME:       %s\n", 
           (envUser != NULL) ? envUser : "未设置");
    
    /* 通过实际用户ID获取 */
    uid = getuid();
    pwd = getpwuid(uid);
    printf("getpwuid(getuid()): %s (UID: %d)\n", 
           (pwd != NULL) ? pwd->pw_name : "NULL", uid);
    
    /* 通过有效用户ID获取 */
    uid = geteuid();
    pwd = getpwuid(uid);
    printf("getpwuid(geteuid()): %s (EUID: %d)\n", 
           (pwd != NULL) ? pwd->pw_name : "NULL", uid);
    
    printf("\n");
}

static void showCurrentTerminal(void) {
    char *tty;
    
    printf("=== 当前终端信息 ===\n");
    
    tty = ttyname(STDIN_FILENO);
    if (tty != NULL) {
        printf("标准输入终端: %s\n", tty);
    } else {
        printf("标准输入不是终端设备\n");
    }
    
    if (isatty(STDIN_FILENO)) {
        printf("标准输入是TTY: 是\n");
    } else {
        printf("标准输入是TTY: 否\n");
    }
    
    printf("\n");
}

static void showUtmpInfo(void) {
    struct utmpx *ut;
    char *loginName;
    char *tty;
    int found = 0;
    
    printf("=== UTMP 文件中的登录信息 ===\n");
    
    loginName = getlogin();
    tty = ttyname(STDIN_FILENO);
    
    if (tty != NULL) {
        /* 去掉路径前缀，只保留终端名 */
        char *ttyBase = strrchr(tty, '/');
        if (ttyBase != NULL) {
            ttyBase++;
        } else {
            ttyBase = tty;
        }
        
        printf("查找终端 %s 的utmp记录...\n", ttyBase);
        
        setutxent();
        while ((ut = getutxent()) != NULL) {
            if (ut->ut_type == USER_PROCESS && 
                strcmp(ut->ut_line, ttyBase) == 0) {
                
                char timeStr[64];
                struct tm *tm = localtime(&ut->ut_tv.tv_sec);
                strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
                
                printf("找到匹配记录:\n");
                printf("  用户: %s\n", ut->ut_user);
                printf("  终端: %s\n", ut->ut_line);
                printf("  主机: %s\n", ut->ut_host);
                printf("  时间: %s\n", timeStr);
                printf("  PID: %d\n", ut->ut_pid);
                found = 1;
                break;
            }
        }
        endutxent();
        
        if (!found) {
            printf("未在utmp中找到当前终端的记录\n");
        }
    } else {
        printf("无法确定当前终端\n");
    }
    
    printf("\n");
}

static void usageError(const char *progName) {
    fprintf(stderr, "Usage: %s [options]\n", progName);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -a         显示所有信息 (默认)\n");
    fprintf(stderr, "  -l         只演示 getlogin()\n");
    fprintf(stderr, "  -r         只演示 getlogin_r()\n");
    fprintf(stderr, "  -s         测试小缓冲区\n");
    fprintf(stderr, "  -c         比较不同方法\n");
    fprintf(stderr, "  -t         显示终端信息\n");
    fprintf(stderr, "  -u         显示utmp信息\n");
    fprintf(stderr, "  -h         显示帮助信息\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "说明:\n");
    fprintf(stderr, "  getlogin() 返回与当前控制终端关联的登录名\n");
    fprintf(stderr, "  这可能与当前用户的实际或有效用户ID不同\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int opt;
    int showAll = 1;
    int showGetlogin = 0;
    int showGetloginR = 0;
    int showSmallBuf = 0;
    int showCompare = 0;
    int showTerminal = 0;
    int showUtmp = 0;
    
    /* 处理命令行参数 */
    while ((opt = getopt(argc, argv, "alrscuth")) != -1) {
        switch (opt) {
        case 'a':
            showAll = 1;
            break;
        case 'l':
            showGetlogin = 1;
            showAll = 0;
            break;
        case 'r':
            showGetloginR = 1;
            showAll = 0;
            break;
        case 's':
            showSmallBuf = 1;
            showAll = 0;
            break;
        case 'c':
            showCompare = 1;
            showAll = 0;
            break;
        case 't':
            showTerminal = 1;
            showAll = 0;
            break;
        case 'u':
            showUtmp = 1;
            showAll = 0;
            break;
        case 'h':
            usageError(argv[0]);
            break;
        default:
            usageError(argv[0]);
        }
    }
    
    printf("getlogin() 和 getlogin_r() 函数演示\n");
    printf("====================================\n\n");
    
    if (showAll || showTerminal) {
        showCurrentTerminal();
    }
    
    if (showAll || showGetlogin) {
        demonstrateGetlogin();
    }
    
    if (showAll || showGetloginR) {
        demonstrateGetloginR();
    }
    
    if (showAll || showSmallBuf) {
        demonstrateSmallBuffer();
    }
    
    if (showAll || showCompare) {
        compareWithOtherMethods();
    }
    
    if (showAll || showUtmp) {
        showUtmpInfo();
    }
    
    printf("注意事项:\n");
    printf("- getlogin() 依赖于控制终端和utmp文件\n");
    printf("- 在某些环境下(如cron、daemon)可能返回NULL\n");
    printf("- getlogin_r() 是线程安全版本\n");
    printf("- 登录名可能与当前用户ID对应的用户名不同\n");
    
    exit(EXIT_SUCCESS);
}
