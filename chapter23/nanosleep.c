#include "tlpi_hdr.h"
#include <time.h>

int main(int argc, char *argv[]) {
  struct timespec req, rem;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s secs\n", argv[0]);
  }

  req.tv_sec = getInt(argv[1], 0, "secs");
  req.tv_nsec = 0;

  printf("NanoSleep for %ld.%09ld\n", (long)req.tv_sec, req.tv_nsec);

  for (;;) {
    errno = 0;
    rem.tv_sec = 0;
    rem.tv_nsec = 0;

    if (nanosleep(&req, &rem) == -1) {
      if (errno == EINTR) {
        printf("NanoSleep interrupted by signal\n");
        break;
      } else {
        errExit("nanosleep");
      }
    }

    printf("Remaining: %ld.%09ld\n", (long)rem.tv_sec, rem.tv_nsec);
  }

  exit(EXIT_SUCCESS);
}