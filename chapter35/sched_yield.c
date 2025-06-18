#include "tlpi_hdr.h"
#include <sched.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 1000000

static void
usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [use-yield]\n", progName);
    fprintf(stderr, "       use-yield should be 'y' or 'n'\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int useYield;
    struct timespec start, end;
    long long elapsedNsec;
    int s;

    if (argc != 2 || strlen(argv[1]) != 1 ||
            (argv[1][0] != 'y' && argv[1][0] != 'n'))
        usageError(argv[0]);

    useYield = argv[1][0] == 'y';

    printf("Executing %d loops %s sched_yield()\n",
            ITERATIONS, useYield ? "with" : "without");

    /* 获取开始时间 */
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
        errExit("clock_gettime");

    /* 执行指定次数的循环 */
    for (int j = 0; j < ITERATIONS; j++) {
        if (useYield) {
            s = sched_yield();
            if (s == -1)
                errExit("sched_yield");
        }
    }

    /* 获取结束时间 */
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
        errExit("clock_gettime");

    /* 计算经过的时间（纳秒） */
    elapsedNsec = (end.tv_sec - start.tv_sec) * 1000000000 +
                  (end.tv_nsec - start.tv_nsec);

    printf("Elapsed time: %.6f seconds\n", elapsedNsec / 1000000000.0);

    exit(EXIT_SUCCESS);
}
