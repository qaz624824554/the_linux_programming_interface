#include <stdio.h>
#define _BSD_SOURCE
#include "file_perms.h"
#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

static void displayStatInfo(const struct stat *buf) {
  printf("File type:                    ");

  switch (buf->st_mode & S_IFMT) {
  case S_IFREG:
    printf("regular file\n");
    break;
  case S_IFDIR:
    printf("directory\n");
    break;
  case S_IFCHR:
    printf("character device\n");
    break;
  case S_IFBLK:
    printf("block device\n");
    break;
  case S_IFIFO:
    printf("FIFO/pipe\n");
    break;
  case S_IFLNK:
    printf("symbolic link\n");
    break;
  case S_IFSOCK:
    printf("socket\n");
    break;
  default:
    printf("unknown file type\n");
    break;
  }

  printf("    i-node number:          %ld\n", (long)buf->st_ino);

  printf("Mode:                %lo (%s)\n", (unsigned long)buf->st_mode,
         filePermStr(buf->st_mode, 0));

  if (buf->st_mode & (S_ISUID | S_ISGID | S_ISVTX)) {
    printf("    special bits set:       %s%s%s\n",
           (buf->st_mode & S_ISUID) ? "set-UID" : "",
           (buf->st_mode & S_ISGID) ? "set-GID" : "",
           (buf->st_mode & S_ISVTX) ? "sticky" : "");
  }

  printf("Number of (hard) links:    %ld\n", (long)buf->st_nlink);
  printf("Ownership:                UID=%ld   GID=%ld\n", (long)buf->st_uid,
         (long)buf->st_gid);

  printf("File size:                %lld bytes\n", (long long)buf->st_size);
  printf("Optimal I/O block size:    %ld bytes\n", (long)buf->st_blksize);
  printf("512B blocks allocated:     %lld\n", (long long)buf->st_blocks);
  printf("Last file access:          %s", ctime(&buf->st_atime));
  printf("Last file modification:    %s", ctime(&buf->st_mtime));
  printf("Last status change:        %s", ctime(&buf->st_ctime));
}

int main(int argc, char *argv[]) {
  struct stat sb;
  Boolean statLink;
  int fname;

  statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;
  fname = statLink ? 2 : 1;

  if (fname >= argc || (argc > 1 && strcmp(argv[1], "--help") == 0)) {
    usageErr("%s [-l] file\n"
             "               -l = use lstat() instead of stat()\n",
             argv[0]);
  }

  if (statLink) {
    if (lstat(argv[fname], &sb) == -1)
      errExit("lstat");
  } else {
    if (stat(argv[fname], &sb) == -1)
      errExit("stat");
  }

  displayStatInfo(&sb);
  exit(EXIT_SUCCESS);
}