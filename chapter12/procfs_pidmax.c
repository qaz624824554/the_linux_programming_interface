#include "tlpi_hdr.h"
#include <fcntl.h>

#define MAX_LINE 1000

int main(int argc, char *argv[]) {
  int fd;
  char line[MAX_LINE];
  ssize_t n;

  fd = open("/proc/sys/kernel/pid_max", (argc > 1) ? O_RDWR : O_RDONLY);
  if (fd == -1) {
    errExit("open");
  }

  n = read(fd, line, MAX_LINE);
  if (n == -1) {
    errExit("read");
  }

  if (argc > 1) {
    printf("Old value: ");
  }

  printf("%.*s\n", (int) n, line);

  if (argc > 1) {
    if (write(fd, argv[1], strlen(argv[1])) != strlen(argv[1])) {
      fatal("write() failed");
    }

    system("echo /proc/sys/kernel/pid_max now contains "
           "`cat /proc/sys/kernel/pid_max`");
  }

  exit(EXIT_SUCCESS);
}