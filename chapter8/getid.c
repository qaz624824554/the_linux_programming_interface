#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    uid_t uid = getuid();
    printf("uid: %d\n", uid);
    uid_t gid = getgid();
    printf("gid: %d\n", gid);
    
    uid_t euid = geteuid();
    printf("euid: %d\n", euid);
    uid_t egid = getegid();
    printf("egid: %d\n", egid);
}