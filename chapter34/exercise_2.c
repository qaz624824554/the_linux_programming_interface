#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main() {
    pid_t child_pid, child_pgid;
    int status;

    printf("Parent PID: %ld, PGID: %ld\n", (long)getpid(), (long)getpgrp());

    // Create first child
    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Child process
        printf("Child 1 before setpgid: PID=%ld, PGID=%ld\n", 
               (long)getpid(), (long)getpgrp());
        
        // Sleep to allow parent to change our PGID
        sleep(2);
        
        printf("Child 1 after parent setpgid: PID=%ld, PGID=%ld\n", 
               (long)getpid(), (long)getpgrp());
        
        exit(EXIT_SUCCESS);
    }

    // Parent process - change child's PGID before it executes
    printf("Parent changing Child 1's PGID before child completes\n");
    if (setpgid(child_pid, child_pid) == -1) {
        printf("Failed to change Child 1's PGID: %s\n", strerror(errno));
    } else {
        printf("Successfully changed Child 1's PGID\n");
    }

    // Create second child
    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Child process
        printf("Child 2 before exec: PID=%ld, PGID=%ld\n", 
               (long)getpid(), (long)getpgrp());
        
        // Execute a new program
        execl("/bin/sleep", "sleep", "10", NULL);
        
        // Should not reach here unless exec fails
        perror("execl");
        exit(EXIT_FAILURE);
    }

    // Give child time to execute
    sleep(1);
    
    // Try to change PGID after exec
    printf("Parent trying to change Child 2's PGID after exec\n");
    if (setpgid(child_pid, child_pid) == -1) {
        printf("Failed to change Child 2's PGID: %s\n", strerror(errno));
    } else {
        printf("Successfully changed Child 2's PGID\n");
    }

    // Wait for both children
    while (wait(&status) > 0);

    return 0;
}
