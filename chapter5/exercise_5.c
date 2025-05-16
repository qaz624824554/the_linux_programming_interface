#include "../lib/tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int fd, dupfd;
    int flags, dup_flags;
    off_t offset;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s filename\n", argv[0]);

    /* Open a file */
    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open");

    /* Create a duplicate file descriptor */
    dupfd = dup(fd);
    if (dupfd == -1)
        errExit("dup");

    /* Get and display initial file offset for both descriptors */
    offset = lseek(fd, 0, SEEK_CUR);
    if (offset == -1)
        errExit("lseek");
    printf("Initial file offset in fd: %lld\n", (long long) offset);

    offset = lseek(dupfd, 0, SEEK_CUR);
    if (offset == -1)
        errExit("lseek");
    printf("Initial file offset in dupfd: %lld\n", (long long) offset);

    /* Change offset using the original descriptor and verify in both */
    if (lseek(fd, 10, SEEK_SET) == -1)
        errExit("lseek");
    printf("Changed offset to 10 using fd\n");

    offset = lseek(fd, 0, SEEK_CUR);
    printf("Current offset in fd: %lld\n", (long long) offset);

    offset = lseek(dupfd, 0, SEEK_CUR);
    printf("Current offset in dupfd: %lld\n", (long long) offset);

    /* Get and display file status flags for both descriptors */
    flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        errExit("fcntl");
    printf("File status flags in fd: %d\n", flags);

    dup_flags = fcntl(dupfd, F_GETFL);
    if (dup_flags == -1)
        errExit("fcntl");
    printf("File status flags in dupfd: %d\n", dup_flags);

    /* Change file status flags using the duplicate descriptor */
    if (fcntl(dupfd, F_SETFL, flags | O_APPEND) == -1)
        errExit("fcntl");
    printf("Added O_APPEND flag using dupfd\n");

    /* Verify flags are changed in both descriptors */
    flags = fcntl(fd, F_GETFL);
    if (flags == -1)
        errExit("fcntl");
    printf("File status flags in fd after change: %d\n", flags);

    dup_flags = fcntl(dupfd, F_GETFL);
    if (dup_flags == -1)
        errExit("fcntl");
    printf("File status flags in dupfd after change: %d\n", dup_flags);

    /* Clean up */
    if (close(fd) == -1)
        errExit("close fd");
    if (close(dupfd) == -1)
        errExit("close dupfd");

    exit(EXIT_SUCCESS);
}
