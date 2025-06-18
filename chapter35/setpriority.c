#include "tlpi_hdr.h"
#include <sys/resource.h>

int main(int argc, char *argv[]) {
    int which, prio, new_prio;
    id_t who;

    if (argc != 4 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s {p|g|u} who priority\n"
                "    p = PRIO_PROCESS\n"
                "    g = PRIO_PGRP\n"
                "    u = PRIO_USER\n", argv[0]);
    }

    /* 解析命令行参数 */
    switch (argv[1][0]) {
    case 'p': which = PRIO_PROCESS; break;
    case 'g': which = PRIO_PGRP;    break;
    case 'u': which = PRIO_USER;    break;
    default:  usageErr("%s: 无效的 'which' 值\n", argv[0]);
    }

    who = getLong(argv[2], 0, "who");
    prio = getInt(argv[3], 0, "prio");

    /* 获取当前优先级 */
    errno = 0;
    new_prio = getpriority(which, who);
    if (new_prio == -1 && errno != 0)
        errExit("getpriority");
    
    printf("原始优先级值: %d\n", new_prio);

    /* 设置新的优先级 */
    if (setpriority(which, who, prio) == -1)
        errExit("setpriority");
    
    /* 验证优先级已更改 */
    errno = 0;
    new_prio = getpriority(which, who);
    if (new_prio == -1 && errno != 0)
        errExit("getpriority");
    
    printf("新的优先级值: %d\n", new_prio);
    
    exit(EXIT_SUCCESS);
}
