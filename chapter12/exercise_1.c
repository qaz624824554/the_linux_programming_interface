#include "tlpi_hdr.h"
#include "ugid_functions.h"
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_LINE 1000

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s user-name\n", argv[0]);
  }

  uid_t uid = userIdFromName(argv[1]);
  if (uid == -1) {
    fatal("Could not obtain uid for user: %s", argv[1]);
  }

  DIR *dir = opendir("/proc");
  if (dir == NULL) {
    errExit("opendir");
  }

  printf("Processes for user '%s' (UID=%ld):\n", argv[1], (long) uid);
  printf("%-8s %s\n", "PID", "Command");
  printf("--------  ---------------\n");

  while (1) {
    errno = 0;
    struct dirent *entry = readdir(dir);
    if (entry == NULL) {
      if (errno == 0) {
        break;
      }
      errExit("readdir");
    }

    /* Skip non-numeric entries - not process directories */
    if (entry->d_name[0] < '0' || entry->d_name[0] > '9') {
      continue;
    }

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
    
    /* Open the status file - if it fails, just continue (process may have terminated) */
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
      continue;  /* Process may have terminated */
    }

    /* Read the status file */
    char buffer[MAX_LINE];
    ssize_t numRead = read(fd, buffer, MAX_LINE - 1);
    if (numRead <= 0) {  /* Empty or failed read */
      close(fd);
      continue;
    }
    
    buffer[numRead] = '\0';  /* Null-terminate the buffer */
    
    /* Extract the command name and UID from the status file */
    char *nameLinePtr = strstr(buffer, "Name:");
    char *uidLinePtr = strstr(buffer, "Uid:");
    
    if (nameLinePtr != NULL && uidLinePtr != NULL) {
      /* Extract the first UID from the Uid line (real UID) */
      char *uidStart = uidLinePtr + 5;  /* Skip "Uid:" */
      while (isspace(*uidStart)) {
        uidStart++;
      }
      
      uid_t processUid = 0;
      char *endptr;
      processUid = strtol(uidStart, &endptr, 10);
      
      /* If this process belongs to the target user */
      if (processUid == uid) {
        /* Extract command name */
        char *nameStart = nameLinePtr + 5;  /* Skip "Name:" */
        while (isspace(*nameStart)) {
          nameStart++;
        }
        
        char cmdName[256] = {0};
        int i = 0;
        while (!isspace(*nameStart) && *nameStart != '\0' && i < 255) {
          cmdName[i++] = *nameStart++;
        }
        cmdName[i] = '\0';
        
        printf("%-8s %s\n", entry->d_name, cmdName);
      }
    }
    
    close(fd);
  }

  closedir(dir);
  exit(EXIT_SUCCESS);
}