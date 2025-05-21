#include "tlpi_hdr.h"
#include <pwd.h>
#include <stdio.h>

int main() {
  struct passwd * passwd;
  passwd = getpwnam("root");

  printf("name: %s\n", passwd->pw_name);
  printf("password: %s\n", passwd->pw_passwd);
  printf("uid: %d\n", passwd->pw_uid);
  printf("gid: %d\n", passwd->pw_gid);
  printf("gecos: %s\n", passwd->pw_gecos);
  printf("dir: %s\n", passwd->pw_dir);
  printf("shell: %s\n", passwd->pw_shell);
}