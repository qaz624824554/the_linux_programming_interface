#include <grp.h>
#include <stdio.h>

int main() {
  struct group* group;

  group = getgrnam("root");

  printf("group name: %s\n", group->gr_name);
  printf("group password: %s\n", group->gr_passwd);
  printf("group id: %d\n", group->gr_gid);
  printf("group members: %s\n", group->gr_mem[0]);
}