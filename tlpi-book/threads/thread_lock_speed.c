/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 33 */

#include <pthread.h>
#include <stdbool.h>
#include "tlpi_hdr.h"

static volatile int glob = 0;
static pthread_spinlock_t splock;
static pthread_mutex_t mtx;
static bool useMutex;
static int numOuterLoops;
static int numInnerLoops;

static void *
threadFunc(void *arg)
{
    int s;

    for (int j = 0; j < numOuterLoops; j++) {
        if (useMutex) {
            s = pthread_mutex_lock(&mtx);
            if (s != 0)
                errExitEN(s, "pthread_mutex_lock");
        } else {
            s = pthread_spin_lock(&splock);
            if (s != 0)
                errExitEN(s, "pthread_spin_lock");
        }

        for (int k = 0; k < numInnerLoops; k++)
            glob++;

        if (useMutex) {
            s = pthread_mutex_unlock(&mtx);
            if (s != 0)
                errExitEN(s, "pthread_mutex_unlock");
        } else {
            s = pthread_spin_unlock(&splock);
            if (s != 0)
                errExitEN(s, "pthread_spin_unlock");
        }
    }

    return NULL;
}

static void
usageError(char *pname)
{
    fprintf(stderr,
            "Usage: %s [-s] num-threads "
            "[num-inner-loops [num-outer-loops]]\n", pname);
    fprintf(stderr,
            "    -q   Don't print verbose messages\n");
    fprintf(stderr,
            "    -s   Use spin locks (instead of the default mutexes)\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    /* Prevent runaway/forgotten process from burning up CPU time forever */

    alarm(120);         /* Unhandled SIGALRM will kill process */

    useMutex = true;
    bool verbose = true;
    int opt;
    while ((opt = getopt(argc, argv, "qs")) != -1) {
        switch (opt) {
        case 'q':
            verbose = false;
            break;
        case 's':
            useMutex = false;
            break;
        default:
            usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    int numThreads = atoi(argv[optind]);
    numInnerLoops = (optind + 1 < argc) ? atoi(argv[optind + 1]) : 1;
    numOuterLoops = (optind + 2 < argc) ? atoi(argv[optind + 2]) : 10000000;

    if (verbose) {
        printf("Using %s\n", useMutex ? "mutexes" : "spin locks");
        printf("\tthreads: %d; outer loops: %d; inner loops: %d\n",
                numThreads, numOuterLoops, numInnerLoops);
    }

    pthread_t *thread = calloc(numThreads, sizeof(pthread_t));
    if (thread == NULL)
        errExit("calloc");

    int s;

    if (useMutex) {
        s = pthread_mutex_init(&mtx, NULL);
        if (s != 0)
            errExitEN(s, "pthread_mutex_init");
    } else {
        s = pthread_spin_init(&splock, 0);
        if (s != 0)
            errExitEN(s, "pthread_spin_init");
    }

    for (int j = 0; j < numThreads; j++) {
        s = pthread_create(&thread[j], NULL, threadFunc, NULL);
        if (s != 0)
            errExitEN(s, "pthread_create");
    }

    for (int j = 0; j < numThreads; j++) {
        s = pthread_join(thread[j], NULL);
        if (s != 0)
            errExitEN(s, "pthread_join");
    }

    if (verbose)
        printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
