/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* 演示动态链接库函数的使用: dlopen, dlerror, dlsym, dlclose, dladdr */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tlpi_hdr.h"

static void demonstrateDlopen(void) {
    void *handle;
    char *error;
    
    printf("=== 演示 dlopen() 和 dlerror() ===\n");
    
    /* 清除之前的错误 */
    dlerror();
    
    /* 尝试打开数学库 */
    printf("正在打开数学库 (libm.so.6)...\n");
    handle = dlopen("libm.so.6", RTLD_LAZY);
    if (!handle) {
        printf("dlopen 失败: %s\n", dlerror());
        return;
    }
    printf("成功打开数学库\n");
    
    /* 关闭库 */
    if (dlclose(handle) != 0) {
        printf("dlclose 失败: %s\n", dlerror());
    } else {
        printf("成功关闭数学库\n");
    }
    printf("\n");
}

static void demonstrateDlsym(void) {
    void *handle;
    double (*cosine)(double);
    char *error;
    double result;
    
    printf("=== 演示 dlsym() 获取函数符号 ===\n");
    
    /* 打开数学库 */
    handle = dlopen("libm.so.6", RTLD_LAZY);
    if (!handle) {
        printf("无法打开数学库: %s\n", dlerror());
        return;
    }
    
    /* 清除错误 */
    dlerror();
    
    /* 获取 cos 函数的地址 */
    printf("正在获取 cos 函数符号...\n");
    cosine = (double (*)(double)) dlsym(handle, "cos");
    
    /* 检查错误 */
    error = dlerror();
    if (error != NULL) {
        printf("dlsym 失败: %s\n", error);
        dlclose(handle);
        return;
    }
    
    /* 调用函数 */
    result = (*cosine)(0.0);
    printf("cos(0.0) = %f\n", result);
    
    result = (*cosine)(M_PI);
    printf("cos(PI) = %f\n", result);
    
    dlclose(handle);
    printf("\n");
}

static void demonstrateDladdr(void) {
    Dl_info info;
    void *handle;
    void *symbol;
    
    printf("=== 演示 dladdr() 获取符号信息 ===\n");
    
    /* 获取当前函数的信息 */
    printf("获取当前函数 (demonstrateDladdr) 的信息:\n");
    if (dladdr((void *)demonstrateDladdr, &info) != 0) {
        printf("  文件名: %s\n", info.dli_fname ? info.dli_fname : "(unknown)");
        printf("  基地址: %p\n", info.dli_fbase);
        printf("  符号名: %s\n", info.dli_sname ? info.dli_sname : "(unknown)");
        printf("  符号地址: %p\n", info.dli_saddr);
    } else {
        printf("dladdr 失败\n");
    }
    
    /* 获取标准库函数的信息 */
    printf("\n获取 printf 函数的信息:\n");
    if (dladdr((void *)printf, &info) != 0) {
        printf("  文件名: %s\n", info.dli_fname ? info.dli_fname : "(unknown)");
        printf("  基地址: %p\n", info.dli_fbase);
        printf("  符号名: %s\n", info.dli_sname ? info.dli_sname : "(unknown)");
        printf("  符号地址: %p\n", info.dli_saddr);
    } else {
        printf("dladdr 失败\n");
    }
    
    /* 从动态库中获取符号信息 */
    handle = dlopen("libm.so.6", RTLD_LAZY);
    if (handle) {
        symbol = dlsym(handle, "sin");
        if (symbol) {
            printf("\n获取 sin 函数的信息:\n");
            if (dladdr(symbol, &info) != 0) {
                printf("  文件名: %s\n", info.dli_fname ? info.dli_fname : "(unknown)");
                printf("  基地址: %p\n", info.dli_fbase);
                printf("  符号名: %s\n", info.dli_sname ? info.dli_sname : "(unknown)");
                printf("  符号地址: %p\n", info.dli_saddr);
            } else {
                printf("dladdr 失败\n");
            }
        }
        dlclose(handle);
    }
    printf("\n");
}

static void demonstrateErrorHandling(void) {
    void *handle;
    void *symbol;
    char *error;
    
    printf("=== 演示错误处理 ===\n");
    
    /* 尝试打开不存在的库 */
    printf("尝试打开不存在的库...\n");
    handle = dlopen("libnonexistent.so", RTLD_LAZY);
    if (!handle) {
        printf("预期错误: %s\n", dlerror());
    }
    
    /* 尝试从不存在的库中获取符号 */
    handle = dlopen("libm.so.6", RTLD_LAZY);
    if (handle) {
        printf("\n尝试获取不存在的符号...\n");
        dlerror(); /* 清除之前的错误 */
        
        symbol = dlsym(handle, "nonexistent_function");
        error = dlerror();
        if (error != NULL) {
            printf("预期错误: %s\n", error);
        }
        
        dlclose(handle);
    }
    printf("\n");
}

static void demonstrateRTLDFlags(void) {
    void *handle1, *handle2;
    
    printf("=== 演示不同的 RTLD 标志 ===\n");
    
    /* RTLD_LAZY vs RTLD_NOW */
    printf("使用 RTLD_LAZY 打开库:\n");
    handle1 = dlopen("libm.so.6", RTLD_LAZY);
    if (handle1) {
        printf("  成功 (延迟绑定)\n");
        dlclose(handle1);
    } else {
        printf("  失败: %s\n", dlerror());
    }
    
    printf("使用 RTLD_NOW 打开库:\n");
    handle2 = dlopen("libm.so.6", RTLD_NOW);
    if (handle2) {
        printf("  成功 (立即绑定)\n");
        dlclose(handle2);
    } else {
        printf("  失败: %s\n", dlerror());
    }
    
    /* 演示 RTLD_GLOBAL */
    printf("\n使用 RTLD_GLOBAL 打开库:\n");
    handle1 = dlopen("libm.so.6", RTLD_LAZY | RTLD_GLOBAL);
    if (handle1) {
        printf("  成功 (全局符号表)\n");
        dlclose(handle1);
    } else {
        printf("  失败: %s\n", dlerror());
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    printf("动态链接库函数演示程序\n");
    printf("=========================\n\n");
    
    demonstrateDlopen();
    demonstrateDlsym();
    demonstrateDladdr();
    demonstrateErrorHandling();
    demonstrateRTLDFlags();
    
    printf("演示完成\n");
    
    exit(EXIT_SUCCESS);
}
