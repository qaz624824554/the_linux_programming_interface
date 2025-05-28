#include "tlpi_hdr.h"
#include <sys/statvfs.h>

int main(int argc, char *argv[])
{
    struct statvfs fsinfo;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s path\n", argv[0]);

    if (statvfs(argv[1], &fsinfo) == -1)
        errExit("statvfs");

    printf("块大小: %ld bytes\n", (long)fsinfo.f_bsize);
    printf("碎片大小: %ld bytes\n", (long)fsinfo.f_frsize);
    printf("块数: %ld\n", (long)fsinfo.f_blocks);
    printf("可用块数: %ld\n", (long)fsinfo.f_bavail);
    printf("可用文件数: %ld\n", (long)fsinfo.f_favail);
    printf("最大文件名长度: %ld\n", (long)fsinfo.f_namemax);
    printf("文件系统ID: %ld\n", (long)fsinfo.f_fsid);
    printf("文件系统挂载选项: %ld\n", (long)fsinfo.f_flag);
    printf("文件系统块大小: %ld\n", (long)fsinfo.f_bsize);
    printf("文件系统块数: %ld\n", (long)fsinfo.f_blocks);
    printf("inode节点数: %ld\n", (long)fsinfo.f_files);
    printf("可用inode节点数: %ld\n", (long)fsinfo.f_ffree);

    exit(EXIT_SUCCESS);
}