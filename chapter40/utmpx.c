/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* 演示解析 utmp 和 wtmp 文件 */

#define _GNU_SOURCE

#include <utmpx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "tlpi_hdr.h"

static void printUtmpEntry(struct utmpx *ut) {
    char timeStr[64];
    
    /* 格式化时间戳 */
    struct tm *tm = localtime(&ut->ut_tv.tv_sec);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
    
    printf("%-12s ", ut->ut_user);
    printf("%-12s ", ut->ut_line);
    printf("%-16s ", ut->ut_host);
    printf("%-20s ", timeStr);
    printf("PID:%-8d ", ut->ut_pid);
    
    /* 显示记录类型 */
    switch (ut->ut_type) {
    case EMPTY:
        printf("EMPTY");
        break;
    case RUN_LVL:
        printf("RUN_LVL");
        break;
    case BOOT_TIME:
        printf("BOOT_TIME");
        break;
    case NEW_TIME:
        printf("NEW_TIME");
        break;
    case OLD_TIME:
        printf("OLD_TIME");
        break;
    case INIT_PROCESS:
        printf("INIT_PROCESS");
        break;
    case LOGIN_PROCESS:
        printf("LOGIN_PROCESS");
        break;
    case USER_PROCESS:
        printf("USER_PROCESS");
        break;
    case DEAD_PROCESS:
        printf("DEAD_PROCESS");
        break;
    default:
        printf("UNKNOWN(%d)", ut->ut_type);
        break;
    }
    printf("\n");
}

static void displayUtmpFile(const char *filename) {
    struct utmpx *ut;
    int entryCount = 0;
    
    printf("\n=== 解析文件: %s ===\n", filename);
    printf("%-12s %-12s %-16s %-20s %-12s %s\n",
           "用户", "终端", "主机", "时间", "PID", "类型");
    printf("================================================================================\n");
    
    /* 设置utmp文件路径 */
    if (utmpxname(filename) == -1) {
        errMsg("utmpxname");
        return;
    }
    
    /* 设置到文件开头 */
    setutxent();
    
    /* 读取所有记录 */
    while ((ut = getutxent()) != NULL) {
        /* 只显示有意义的记录 */
        if (ut->ut_type == USER_PROCESS || 
            ut->ut_type == DEAD_PROCESS ||
            ut->ut_type == BOOT_TIME ||
            ut->ut_type == RUN_LVL ||
            ut->ut_type == LOGIN_PROCESS) {
            printUtmpEntry(ut);
            entryCount++;
        }
    }
    
    endutxent();
    printf("总共显示了 %d 条记录\n", entryCount);
}

static void searchUser(const char *filename, const char *username) {
    struct utmpx *ut;
    int found = 0;
    
    printf("\n=== 搜索用户 '%s' 在文件 %s 中的记录 ===\n", username, filename);
    
    if (utmpxname(filename) == -1) {
        errMsg("utmpxname");
        return;
    }
    
    setutxent();
    
    while ((ut = getutxent()) != NULL) {
        if (strcmp(ut->ut_user, username) == 0) {
            if (!found) {
                printf("%-12s %-12s %-16s %-20s %-12s %s\n",
                       "用户", "终端", "主机", "时间", "PID", "类型");
                printf("================================================================================\n");
                found = 1;
            }
            printUtmpEntry(ut);
        }
    }
    
    endutxent();
    
    if (!found) {
        printf("未找到用户 '%s' 的记录\n", username);
    }
}

static void displayCurrentUsers(void) {
    struct utmpx *ut;
    int userCount = 0;
    
    printf("\n=== 当前登录的用户 ===\n");
    printf("%-12s %-12s %-16s %-20s %s\n",
           "用户", "终端", "主机", "时间", "PID");
    printf("========================================================================\n");
    
    /* 使用默认的utmp文件 */
    setutxent();
    
    while ((ut = getutxent()) != NULL) {
        if (ut->ut_type == USER_PROCESS) {
            char timeStr[64];
            struct tm *tm = localtime(&ut->ut_tv.tv_sec);
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
            
            printf("%-12s %-12s %-16s %-20s %d\n",
                   ut->ut_user, ut->ut_line, ut->ut_host, timeStr, ut->ut_pid);
            userCount++;
        }
    }
    
    endutxent();
    printf("当前有 %d 个用户登录\n", userCount);
}

static void findLoginLogout(const char *filename, const char *username) {
    struct utmpx *ut;
    
    printf("\n=== 查找用户 '%s' 的登录/登出记录 ===\n", username);
    
    if (utmpxname(filename) == -1) {
        errMsg("utmpxname");
        return;
    }
    
    setutxent();
    
    while ((ut = getutxent()) != NULL) {
        if (strcmp(ut->ut_user, username) == 0 && 
            (ut->ut_type == USER_PROCESS || ut->ut_type == DEAD_PROCESS)) {
            
            char timeStr[64];
            struct tm *tm = localtime(&ut->ut_tv.tv_sec);
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
            
            printf("%s: 用户 %s %s %s (PID: %d)\n",
                   timeStr, username,
                   (ut->ut_type == USER_PROCESS) ? "登录到" : "从",
                   ut->ut_line, ut->ut_pid);
        }
    }
    
    endutxent();
}

static void displayLastBoot(const char *filename) {
    struct utmpx *ut;
    time_t lastBootTime = 0;
    
    printf("\n=== 查找最后一次系统启动时间 ===\n");
    
    if (utmpxname(filename) == -1) {
        errMsg("utmpxname");
        return;
    }
    
    setutxent();
    
    while ((ut = getutxent()) != NULL) {
        if (ut->ut_type == BOOT_TIME) {
            if (ut->ut_tv.tv_sec > lastBootTime) {
                lastBootTime = ut->ut_tv.tv_sec;
            }
        }
    }
    
    endutxent();
    
    if (lastBootTime > 0) {
        struct tm *tm = localtime(&lastBootTime);
        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
        printf("最后一次系统启动时间: %s\n", timeStr);
    } else {
        printf("未找到系统启动记录\n");
    }
}

static void usageError(const char *progName) {
    fprintf(stderr, "Usage: %s [options]\n", progName);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -f file    指定utmp/wtmp文件路径\n");
    fprintf(stderr, "  -u user    搜索指定用户的记录\n");
    fprintf(stderr, "  -c         显示当前登录用户\n");
    fprintf(stderr, "  -l user    查找用户的登录/登出记录\n");
    fprintf(stderr, "  -b         显示最后启动时间\n");
    fprintf(stderr, "  -h         显示帮助信息\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "示例:\n");
    fprintf(stderr, "  %s                           # 显示当前用户\n", progName);
    fprintf(stderr, "  %s -f /var/log/wtmp          # 解析wtmp文件\n", progName);
    fprintf(stderr, "  %s -u root                   # 搜索root用户记录\n", progName);
    fprintf(stderr, "  %s -l root -f /var/log/wtmp  # 查找root的登录记录\n", progName);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    const char *filename = NULL;
    const char *username = NULL;
    const char *loginUser = NULL;
    int opt;
    int showCurrent = 0;
    int showBoot = 0;
    
    /* 处理命令行参数 */
    while ((opt = getopt(argc, argv, "f:u:cl:bh")) != -1) {
        switch (opt) {
        case 'f':
            filename = optarg;
            break;
        case 'u':
            username = optarg;
            break;
        case 'c':
            showCurrent = 1;
            break;
        case 'l':
            loginUser = optarg;
            break;
        case 'b':
            showBoot = 1;
            break;
        case 'h':
            usageError(argv[0]);
            break;
        default:
            usageError(argv[0]);
        }
    }
    
    printf("UTMP/WTMP 文件解析工具\n");
    printf("=====================\n");
    
    /* 如果没有指定任何选项，显示当前用户 */
    if (!filename && !username && !showCurrent && !loginUser && !showBoot) {
        showCurrent = 1;
    }
    
    /* 显示当前登录用户 */
    if (showCurrent) {
        displayCurrentUsers();
    }
    
    /* 如果指定了文件，解析该文件 */
    if (filename) {
        displayUtmpFile(filename);
        
        if (username) {
            searchUser(filename, username);
        }
        
        if (loginUser) {
            findLoginLogout(filename, loginUser);
        }
        
        if (showBoot) {
            displayLastBoot(filename);
        }
    } else {
        /* 使用默认文件进行搜索 */
        if (username) {
            printf("\n使用默认utmp文件搜索用户...\n");
            searchUser(UTMPX_FILE, username);
        }
        
        if (loginUser) {
            printf("\n在默认wtmp文件中查找登录记录...\n");
            findLoginLogout(WTMPX_FILE, loginUser);
        }
        
        if (showBoot) {
            printf("\n在默认wtmp文件中查找启动时间...\n");
            displayLastBoot(WTMPX_FILE);
        }
    }
    
    exit(EXIT_SUCCESS);
}
