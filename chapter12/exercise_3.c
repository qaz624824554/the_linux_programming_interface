#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>

#define PROC_PATH "/proc"
#define FD_PATH "fd"
#define MAX_PATH 4096

void print_processes_with_file(const char *target_path) {
    DIR *proc_dir, *fd_dir;
    struct dirent *proc_entry, *fd_entry;
    char fd_path[MAX_PATH];
    char link_path[MAX_PATH];
    char real_path[MAX_PATH];
    ssize_t len;
    
    // Open /proc directory
    proc_dir = opendir(PROC_PATH);
    if (proc_dir == NULL) {
        perror("Failed to open /proc directory");
        return;
    }
    
    printf("Processes with open file: %s\n", target_path);
    printf("PID\tCOMMAND\n");
    printf("---\t-------\n");
    
    // Iterate through all PID directories in /proc
    while ((proc_entry = readdir(proc_dir)) != NULL) {
        // Skip non-PID entries (not numeric)
        if (proc_entry->d_name[0] < '0' || proc_entry->d_name[0] > '9')
            continue;
        
        // Construct path to /proc/PID/fd directory
        snprintf(fd_path, MAX_PATH, "%s/%s/%s", PROC_PATH, proc_entry->d_name, FD_PATH);
        
        // Open the fd directory
        fd_dir = opendir(fd_path);
        if (fd_dir == NULL)
            continue;  // Skip if we can't open the directory
        
        // Iterate through all file descriptors
        while ((fd_entry = readdir(fd_dir)) != NULL) {
            // Skip . and ..
            if (strcmp(fd_entry->d_name, ".") == 0 || strcmp(fd_entry->d_name, "..") == 0)
                continue;
            
            // Construct path to the fd symlink
            snprintf(link_path, MAX_PATH, "%s/%s", fd_path, fd_entry->d_name);
            
            // Read the symlink
            len = readlink(link_path, real_path, MAX_PATH - 1);
            if (len == -1)
                continue;  // Skip if readlink fails
            
            real_path[len] = '\0';  // Null-terminate the path
            
            // Check if this file matches our target
            if (strcmp(real_path, target_path) == 0) {
                // Get process name from /proc/PID/comm
                char comm_path[MAX_PATH];
                char comm[256] = {0};
                FILE *comm_file;
                
                snprintf(comm_path, MAX_PATH, "%s/%s/comm", PROC_PATH, proc_entry->d_name);
                comm_file = fopen(comm_path, "r");
                if (comm_file != NULL) {
                    if (fgets(comm, sizeof(comm), comm_file) != NULL) {
                        // Remove trailing newline
                        size_t len = strlen(comm);
                        if (len > 0 && comm[len-1] == '\n')
                            comm[len-1] = '\0';
                    }
                    fclose(comm_file);
                } else {
                    strcpy(comm, "unknown");
                }
                
                printf("%s\t%s\n", proc_entry->d_name, comm);
                break;  // Found a match, no need to check other FDs
            }
        }
        
        closedir(fd_dir);
    }
    
    closedir(proc_dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filepath>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Get absolute path of the target file
    char abs_path[PATH_MAX];
    if (realpath(argv[1], abs_path) == NULL) {
        perror("realpath");
        exit(EXIT_FAILURE);
    }
    
    print_processes_with_file(abs_path);
    
    return 0;
}
