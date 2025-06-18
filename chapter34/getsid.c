#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    pid_t pid, sid;
    pid = getpid();
    sid = getsid(pid);
    printf("进程ID: %ld\n", (long)pid);
    printf("会话ID: %ld\n", (long)sid);
    exit(EXIT_SUCCESS);
}