#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <time.h>
int main(int argc, char *argv[])
{
    struct stat statbuf;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    if (stat(argv[1], &statbuf) == -1)
        errExit("stat");

    printf("File type: ");
    switch (statbuf.st_mode & S_IFMT)
    {
        case S_IFREG: printf("regular file\n"); break;
        case S_IFDIR: printf("directory\n"); break;
        case S_IFCHR: printf("character device\n"); break;
        case S_IFBLK: printf("block device\n"); break;
        case S_IFIFO: printf("FIFO/pipe\n"); break;
        case S_IFLNK: printf("symbolic link\n"); break;
        case S_IFSOCK: printf("socket\n"); break;
        default: printf("unknown file type\n"); break;
    }

    printf("File size: %lld bytes\n", (long long)statbuf.st_size);
    printf("Blocks allocated: %lld\n", (long long)statbuf.st_blocks);
    
    if (S_ISREG(statbuf.st_mode))
        printf("IO block size: %ld bytes\n", (long)statbuf.st_blksize);
    else
        printf("IO block size: not a regular file\n");

    printf("Last status change: %s\n", ctime(&statbuf.st_ctime));
    printf("Last file access: %s\n", ctime(&statbuf.st_atime));
    printf("Last file modification: %s\n", ctime(&statbuf.st_mtime));
    
    // 打印其他信息
    printf("File inode: %ld\n", (long)statbuf.st_ino);
    printf("File device: %ld\n", (long)statbuf.st_dev);
    printf("File links: %ld\n", (long)statbuf.st_nlink);
    printf("File uid: %d\n", statbuf.st_uid);
    printf("File gid: %d\n", statbuf.st_gid);
    printf("File size: %lld bytes\n", (long long)statbuf.st_size);
    printf("File permissions: %o\n", statbuf.st_mode & 0777);
    
}