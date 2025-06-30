#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

/* 演示 popen() 读取命令输出 */
static void demonstratePopenRead(void) {
    FILE *fp;
    char buffer[1024];
    
    printf("=== 演示 popen() 读取命令输出 ===\n");
    
    /* 执行 ls -l 命令并读取输出 */
    printf("执行 'ls -l' 命令:\n");
    fp = popen("ls -l", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    /* 逐行读取命令输出 */
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    
    /* 关闭管道并获取命令退出状态 */
    int status = pclose(fp);
    if (status == -1) {
        perror("pclose 失败");
    } else {
        printf("命令退出状态: %d\n", WEXITSTATUS(status));
    }
    printf("\n");
}

/* 演示 popen() 向命令写入数据 */
static void demonstratePopenWrite(void) {
    FILE *fp;
    const char *data[] = {
        "第一行数据\n",
        "第二行数据\n", 
        "第三行数据\n",
        NULL
    };
    int i;
    
    printf("=== 演示 popen() 向命令写入数据 ===\n");
    
    /* 执行 sort 命令，向其写入数据 */
    printf("向 'sort' 命令写入数据进行排序:\n");
    printf("输入数据:\n");
    for (i = 0; data[i] != NULL; i++) {
        printf("  %s", data[i]);
    }
    
    fp = popen("sort", "w");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    /* 向命令写入数据 */
    for (i = 0; data[i] != NULL; i++) {
        if (fputs(data[i], fp) == EOF) {
            perror("写入数据失败");
            break;
        }
    }
    
    /* 关闭管道，sort 命令会输出排序结果 */
    printf("\n排序后的输出:\n");
    int status = pclose(fp);
    if (status == -1) {
        perror("pclose 失败");
    } else {
        printf("sort 命令退出状态: %d\n", WEXITSTATUS(status));
    }
    printf("\n");
}

/* 演示管道命令组合 */
static void demonstratePipelineCommands(void) {
    FILE *fp;
    char buffer[256];
    
    printf("=== 演示管道命令组合 ===\n");
    
    /* 执行复杂的管道命令：ps aux | grep -v grep | wc -l */
    printf("执行命令: 'ps aux | grep -v grep | wc -l'\n");
    printf("(统计当前运行的进程数量)\n");
    
    fp = popen("ps aux | grep -v grep | wc -l", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("当前运行的进程数量: %s", buffer);
    }
    
    pclose(fp);
    
    /* 另一个例子：ls | wc -l */
    printf("\n执行命令: 'ls | wc -l'\n");
    printf("(统计当前目录文件数量)\n");
    
    fp = popen("ls | wc -l", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("当前目录文件数量: %s", buffer);
    }
    
    pclose(fp);
    printf("\n");
}

/* 演示错误处理 */
static void demonstrateErrorHandling(void) {
    FILE *fp;
    char buffer[256];
    int status;
    
    printf("=== 演示错误处理 ===\n");
    
    /* 尝试执行不存在的命令 */
    printf("尝试执行不存在的命令 'nonexistent_command':\n");
    fp = popen("nonexistent_command", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    /* 尝试读取输出 */
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("意外的输出: %s", buffer);
    } else {
        printf("没有输出 (符合预期)\n");
    }
    
    /* 检查命令退出状态 */
    status = pclose(fp);
    if (status == -1) {
        perror("pclose 失败");
    } else {
        printf("命令退出状态: %d\n", WEXITSTATUS(status));
        if (WEXITSTATUS(status) != 0) {
            printf("命令执行失败\n");
        }
    }
    
    /* 演示命令语法错误 */
    printf("\n尝试执行语法错误的命令 'ls --invalid-option':\n");
    fp = popen("ls --invalid-option", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    /* 读取错误输出（注意：popen 只能捕获 stdout，stderr 仍会显示到终端） */
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("输出: %s", buffer);
    }
    
    status = pclose(fp);
    if (status == -1) {
        perror("pclose 失败");
    } else {
        printf("命令退出状态: %d\n", WEXITSTATUS(status));
    }
    printf("\n");
}

/* 演示与 shell 管道的区别 */
static void demonstrateShellIntegration(void) {
    FILE *fp;
    char buffer[512];
    
    printf("=== 演示 popen() 与 shell 的集成 ===\n");
    
    /* 使用 shell 的变量和功能 */
    printf("使用 shell 变量和功能:\n");
    fp = popen("echo \"当前用户: $USER\"; echo \"当前目录: $PWD\"", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);
    
    /* 使用通配符 */
    printf("\n使用 shell 通配符列出 .c 文件:\n");
    fp = popen("ls *.c 2>/dev/null || echo '没有找到 .c 文件'", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);
    printf("\n");
}

/* 演示大数据量处理 */
static void demonstrateLargeDataProcessing(void) {
    FILE *fp;
    char buffer[1024];
    int line_count = 0;
    
    printf("=== 演示大数据量处理 ===\n");
    
    /* 生成大量数据并处理 */
    printf("生成 1-100 的数字，计算总和:\n");
    fp = popen("seq 1 100 | awk '{sum += $1} END {print \"总和:\", sum}'", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);
    
    /* 处理文件内容 */
    printf("\n分析当前目录下文件的类型分布:\n");
    fp = popen("find . -type f -name '*.*' | sed 's/.*\\.//' | sort | uniq -c | sort -nr", "r");
    if (fp == NULL) {
        perror("popen 失败");
        return;
    }
    
    printf("文件扩展名统计:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL && line_count < 10) {
        printf("  %s", buffer);
        line_count++;
    }
    
    if (line_count == 10) {
        printf("  ... (只显示前10行)\n");
    }
    
    pclose(fp);
    printf("\n");
}

int main(void) {
    printf("=== popen() 和 pclose() 功能演示 ===\n");
    printf("popen() 允许程序执行 shell 命令并通过管道与其通信\n\n");
    
    /* 执行各种演示 */
    demonstratePopenRead();
    demonstratePopenWrite();
    demonstratePipelineCommands();
    demonstrateErrorHandling();
    demonstrateShellIntegration();
    demonstrateLargeDataProcessing();
    
    printf("=== popen() 演示完成 ===\n");
    printf("\n主要特点:\n");
    printf("1. popen() 创建管道并启动 shell 执行命令\n");
    printf("2. 可以读取命令输出或向命令输入数据\n");
    printf("3. pclose() 等待命令完成并返回退出状态\n");
    printf("4. 支持复杂的 shell 管道和重定向\n");
    printf("5. 自动处理 fork/exec/pipe 的复杂性\n");
    
    return 0;
}
