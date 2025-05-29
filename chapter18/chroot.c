#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s path\n", argv[0]);

  if (chroot(argv[1]) == -1)
    errExit("chroot");

  if (chdir("/") == -1)
    errExit("chdir");

  execlp("bash", "bash", (char *)NULL);
  errExit("execlp");
}