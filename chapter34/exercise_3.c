#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    pid_t pid, pgid, sid;
    
    printf("Parent process: PID=%ld, PGID=%ld, SID=%ld\n", 
           (long)getpid(), (long)getpgrp(), (long)getsid(0));
    
    // Create a child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        // Child process
        printf("Child before: PID=%ld, PGID=%ld, SID=%ld\n", 
               (long)getpid(), (long)getpgrp(), (long)getsid(0));
        
        // Try to create a new session as a process group leader
        sid = setsid();
        if (sid == -1) {
            printf("setsid() failed: %s\n", strerror(errno));
        } else {
            printf("setsid() succeeded: new SID=%ld\n", (long)sid);
        }
        
        exit(EXIT_SUCCESS);
    }
    
    // Parent process
    sleep(1); // Give child time to execute
    
    // Make parent a process group leader
    pgid = setpgid(0, 0);
    if (pgid == -1) {
        perror("setpgid");
        exit(EXIT_FAILURE);
    }
    
    printf("Parent is now process group leader: PID=%ld, PGID=%ld\n", 
           (long)getpid(), (long)getpgrp());
    
    // Try to create a new session as a process group leader
    sid = setsid();
    if (sid == -1) {
        printf("setsid() failed for process group leader: %s\n", strerror(errno));
    } else {
        printf("setsid() succeeded: new SID=%ld\n", (long)sid);
    }
    
    return 0;
}
