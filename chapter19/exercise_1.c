#define _XOPEN_SOURCE 600

#include "tlpi_hdr.h"
#include <ftw.h>
#include <limits.h>
#include <sys/inotify.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int inotifyFd;

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

static int dirTree(const char *pathname, const struct stat *sb, int typeflag,
                   struct FTW *ftwbuf) {
  // 如果是目录，则添加到inotify中
  if (S_ISDIR(sb->st_mode)) {
    printf("Adding watch for %s\n", pathname);
    inotify_add_watch(inotifyFd, pathname, IN_ALL_EVENTS);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int flags = 0;
  char buf[BUF_LEN];
  ssize_t numRead;
  char *p;
  struct inotify_event *event;

  inotifyFd = inotify_init();
  if (inotifyFd == -1) {
    errExit("inotify_init");
  }

  if (nftw(argv[1], dirTree, 10, flags) == -1) {
    errExit("nftw");
  }

  while (1) {
    numRead = read(inotifyFd, buf, BUF_LEN);
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