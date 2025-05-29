#include "tlpi_hdr.h"
#include <sys/stat.h>
int main(int argc, char *argv[]) {

  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname\n", argv[0]);

  if (mkdir(argv[1], S_IRWXU | S_IRWXG | S_IRWXO) == -1)
    errExit("mkdir");

  exit(EXIT_SUCCESS);
}
