#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>
#include <time.h>

int main(int argc, char *argv[]) {
  struct utimbuf currentTime;

  struct stat sb;

  if (stat(argv[1], &sb) == -1)
    errExit("stat");

  currentTime.actime = sb.st_atime;
  currentTime.modtime = sb.st_mtime;

  if (utime(argv[1], &currentTime) == -1)
    errExit("utime");

  exit(EXIT_SUCCESS);
}