#define _XOPEN_SOURCE 600

#include "tlpi_hdr.h"
#include <ftw.h>

static int callback(const char *pathname, const struct stat *sb, int typeflag,
                    struct FTW *ftwbuf) {
  printf("pathname=%s; typeflag=%d\n", pathname, typeflag);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname\n", argv[0]);

  int flags = 0;
  flags |= FTW_DEPTH;

  if (nftw(argc > 1 ? argv[1] : ".", callback, 10, flags) == -1)
    errExit("nftw");

  exit(EXIT_SUCCESS);
}