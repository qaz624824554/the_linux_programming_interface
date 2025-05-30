#include "tlpi_hdr.h"
#include <sys/inotify.h>

#define BUF_SIZE 10000

// 监控 argv[1] IN_ATTRIB 事件
int main(int argc, char *argv[]) {
  int inotify_fd, wd;
  char buf[BUF_SIZE];
  ssize_t num_read;
  char *p;
  struct inotify_event *event;

  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pathname\n", argv[0]);

  inotify_fd = inotify_init();
  if (inotify_fd == -1)
    errExit("inotify_init");

  wd = inotify_add_watch(inotify_fd, argv[1], IN_ALL_EVENTS);
  if (wd == -1)
    errExit("inotify_add_watch");

  while (1) {
    num_read = read(inotify_fd, buf, BUF_SIZE);
    if (num_read == 0)
      fatal("read return 0");
    if (num_read == -1)
      errExit("read");

    for (p = buf; p < buf + num_read;) {
      event = (struct inotify_event *)p;
      printf("WD = %d; ", event->wd);
      if (event->mask & IN_ATTRIB)
        printf("IN_ATTRIB ");
      if (event->mask & IN_CLOSE_WRITE)
        printf("IN_CLOSE_WRITE ");
      if (event->mask & IN_DELETE_SELF)
        printf("IN_DELETE_SELF ");
      if (event->mask & IN_MOVE_SELF)
        printf("IN_MOVE_SELF ");
      printf("\n");
      p += sizeof(struct inotify_event) + event->len;
    }
  }
}
