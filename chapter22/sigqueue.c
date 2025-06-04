#include "tlpi_hdr.h"
#include <signal.h>

int main(int argc, char *argv[]) {
  pid_t pid;
  union sigval sv;

  if (argc < 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pid sig [sig-data]\n", argv[0]);

  pid = getLong(argv[1], 0, "pid");
  sv.sival_int = argc > 3 ? getInt(argv[2], 0, "sig-data") : 0;

  // 调用1000次
  for (int i = 0; i < 1000; i++) {
    if (sigqueue(pid, SIGRTMIN + 1, sv) == -1)
      errExit("sigqueue");
  }

  exit(EXIT_SUCCESS);
}