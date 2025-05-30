#include "tlpi_hdr.h"
#include <limits.h>
#include <sys/inotify.h>

static void displayInotifyEvent(struct inotify_event *event) {
  printf("    wd =%2d; ", event->wd);
  if (event->cookie > 0) {
    printf("cookie = %4d; ", event->cookie);
  }

  printf("mask = ");
  if (event->mask & IN_ACCESS)
    printf("IN_ACCESS ");
  if (event->mask & IN_ATTRIB)
    printf("IN_ATTRIB ");
  if (event->mask & IN_CLOSE_NOWRITE)
    printf("IN_CLOSE_NOWRITE ");
  if (event->mask & IN_CLOSE_WRITE)
    printf("IN_CLOSE_WRITE ");
  if (event->mask & IN_CREATE)
    printf("IN_CREATE ");
  if (event->mask & IN_DELETE)
    printf("IN_DELETE ");
  if (event->mask & IN_DELETE_SELF)
    printf("IN_DELETE_SELF ");
  if (event->mask & IN_IGNORED)
    printf("IN_IGNORED ");
  if (event->mask & IN_ISDIR)
    printf("IN_ISDIR ");
  if (event->mask & IN_MODIFY)
    printf("IN_MODIFY ");
  if (event->mask & IN_MOVE_SELF)
    printf("IN_MOVE_SELF ");
  if (event->mask & IN_MOVED_FROM)
    printf("IN_MOVED_FROM ");
  if (event->mask & IN_MOVED_TO)
    printf("IN_MOVED_TO ");
  if (event->mask & IN_OPEN)
    printf("IN_OPEN ");
  if (event->mask & IN_Q_OVERFLOW)
    printf("IN_Q_OVERFLOW ");
  if (event->mask & IN_UNMOUNT)
    printf("IN_UNMOUNT ");

  printf("\n");

  if (event->len > 0) {
    printf("        name = %s\n", event->name);
  }
}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[]) {
  int inotify_fd, wd, j;
  char buf[BUF_LEN];
  ssize_t numRead;
  char *p;
  struct inotify_event *event;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s pathname\n", argv[0]);
  }

  inotify_fd = inotify_init();
  if (inotify_fd == -1) {
    errExit("inotify_init");
  }

  for (j = 1; j < argc; j++) {
    wd = inotify_add_watch(inotify_fd, argv[j], IN_ALL_EVENTS);
    if (wd == -1) {
      errExit("inotify_add_watch");
    }

    printf("Watching %s using wd %d\n", argv[j], wd);
  }

  while (1) {
    numRead = read(inotify_fd, buf, BUF_LEN);
    if (numRead == 0) {
      fatal("read() from inotify fd returned 0");
    }

    if (numRead == -1) {
      errExit("read");
    }

    printf("Read %ld bytes from inotify fd\n", (long)numRead);

    for (p = buf; p < buf + numRead;) {
      event = (struct inotify_event *)p;
      displayInotifyEvent(event);
      p += sizeof(struct inotify_event) + event->len;
    }
  }

  exit(EXIT_SUCCESS);
}