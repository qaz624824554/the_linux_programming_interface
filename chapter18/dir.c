#include "tlpi_hdr.h"
#include <dirent.h>

int main(int argc, char *argv[]) {
  DIR *dirp;

  if (argc > 1 && strcmp(argv[1], "--help") == 0)
    usageErr("%s [dir-path]\n", argv[0]);

  if ((dirp = opendir(argc > 1 ? argv[1] : ".")) == NULL)
    errExit("opendir");

  for (;;) {
    struct dirent *dp;
    errno = 0;

    if ((dp = readdir(dirp)) == NULL) {
      if (errno == 0)
        break;
    }

    printf("d_ino=%ld; d_reclen=%d; d_type=%d; d_name=%s\n", (long)dp->d_ino,
           dp->d_reclen, dp->d_type, dp->d_name);
  }

  // 重置目录流
  rewinddir(dirp);

  // 获取关联的文件描述符
  int fd = dirfd(dirp);
  if (fd == -1)
    errExit("dirfd");

  if (closedir(dirp) == -1)
    errExit("closedir");

  exit(EXIT_SUCCESS);
}