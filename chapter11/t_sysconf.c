#include "tlpi_hdr.h"

static void sysconfPrint(const char *msg, int name) {
  long lim;

  errno = 0;
  lim = sysconf(name);
  if (lim != -1) {
    printf("%s %ld\n", msg, lim);
  } else {
    if (errno == 0) {
      printf("%s (indeterminate)\n", msg);
    } else {
      errExit("sysconf %s", msg);
    }
  }
}

int main(int argc, char *argv[]) {
  sysconfPrint("_SC_ARG_MAX: ", _SC_ARG_MAX);
  sysconfPrint("_SC_LOGIN_NAME_MAX: ", _SC_LOGIN_NAME_MAX);
  sysconfPrint("_SC_OPEN_MAX: ", _SC_OPEN_MAX);
  sysconfPrint("_SC_NGROUPS_MAX: ", _SC_NGROUPS_MAX);
  sysconfPrint("_SC_PAGESIZE: ", _SC_PAGESIZE);
  sysconfPrint("_SC_RTSIG_MAX: ", _SC_RTSIG_MAX);
  sysconfPrint("_SC_SEM_NSEMS_MAX: ", _SC_SEM_NSEMS_MAX);
  sysconfPrint("_SC_SEM_VALUE_MAX: ", _SC_SEM_VALUE_MAX);

  exit(EXIT_SUCCESS);
}