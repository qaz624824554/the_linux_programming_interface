#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_PROCESSES 10000
#define MAX_CMD_LEN 1024
#define MAX_LINE_LEN 256

typedef struct Process {
    int pid;
    int ppid;
    char cmd[MAX_CMD_LEN];
    struct Process *children[MAX_PROCESSES];
    int num_children;
} Process;

Process *processes[MAX_PROCESSES];
int process_count = 0;

// 从/proc/PID/status文件中获取父进程ID
int get_ppid(int pid) {
    char path[256];
    char line[MAX_LINE_LEN];
    int ppid = -1;
    
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return -1;  // 进程可能已经结束
    }
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "PPid:", 5) == 0) {
            ppid = atoi(line + 5);
            break;
        }
    }
    
    fclose(fp);
    return ppid;
}

// 从/proc/PID/cmdline文件中获取命令行
void get_cmdline(int pid, char *cmd, size_t cmd_size) {
    char path[256];
    char buffer[MAX_CMD_LEN];
    
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        snprintf(cmd, cmd_size, "[unknown]");
        return;
    }
    
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';
    
    // cmdline中参数以'\0'分隔，将其替换为空格
    for (size_t i = 0; i < n; i++) {
        if (buffer[i] == '\0') {
            buffer[i] = ' ';
        }
    }
    
    if (n == 0) {
        // 尝试从comm文件获取命令名
        fclose(fp);
        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        fp = fopen(path, "r");
        if (fp != NULL) {
            if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                // 移除尾部的换行符
                size_t len = strlen(buffer);
                if (len > 0 && buffer[len-1] == '\n') {
                    buffer[len-1] = '\0';
                }
            } else {
                snprintf(buffer, sizeof(buffer), "[unknown]");
            }
        } else {
            snprintf(buffer, sizeof(buffer), "[unknown]");
        }
    }
    
    snprintf(cmd, cmd_size, "%s", buffer);
    if (fp) {
        fclose(fp);
    }
}

// 创建进程结构
Process *create_process(int pid) {
    Process *proc = (Process *)malloc(sizeof(Process));
    if (proc == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    proc->pid = pid;
    proc->ppid = get_ppid(pid);
    proc->num_children = 0;
    get_cmdline(pid, proc->cmd, sizeof(proc->cmd));
    
    return proc;
}

// 添加子进程到父进程
void add_child(Process *parent, Process *child) {
    if (parent->num_children < MAX_PROCESSES) {
        parent->children[parent->num_children++] = child;
    }
}

// 递归打印进程树
void print_process_tree(Process *proc, int level) {
    if (proc == NULL) {
        return;
    }
    
    // 打印缩进
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    
    // 打印进程信息
    printf("%d %s\n", proc->pid, proc->cmd);
    
    // 打印子进程
    for (int i = 0; i < proc->num_children; i++) {
        print_process_tree(proc->children[i], level + 1);
    }
}

// 查找进程
Process *find_process(int pid) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i]->pid == pid) {
            return processes[i];
        }
    }
    return NULL;
}

// 构建进程树
void build_process_tree() {
    // 为每个进程找到其父进程并建立关系
    for (int i = 0; i < process_count; i++) {
        Process *proc = processes[i];
        if (proc->ppid > 0) {
            Process *parent = find_process(proc->ppid);
            if (parent != NULL) {
                add_child(parent, proc);
            }
        }
    }
}

int main() {
    DIR *dir;
    struct dirent *entry;
    
    // 打开/proc目录
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }
    
    // 遍历/proc目录，查找所有进程
    while ((entry = readdir(dir)) != NULL) {
        // 检查目录名是否为数字（PID）
        if (entry->d_type == DT_DIR) {
            int is_pid = 1;
            for (const char *p = entry->d_name; *p; p++) {
                if (!isdigit(*p)) {
                    is_pid = 0;
                    break;
                }
            }
            
            if (is_pid) {
                int pid = atoi(entry->d_name);
                Process *proc = create_process(pid);
                if (proc->ppid != -1) {  // 确保进程仍然存在
                    processes[process_count++] = proc;
                } else {
                    free(proc);
                }
            }
        }
    }
    
    closedir(dir);
    
    // 构建进程树
    build_process_tree();
    
    // 查找init进程（PID为1）
    Process *init_process = find_process(1);
    if (init_process != NULL) {
        printf("Process Tree (root: init):\n");
        print_process_tree(init_process, 0);
    } else {
        printf("Could not find init process (PID 1)\n");
    }
    
    // 释放内存
    for (int i = 0; i < process_count; i++) {
        free(processes[i]);
    }
    
    return 0;
}
