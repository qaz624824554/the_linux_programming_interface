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
#include "tlpi_hdr.h"

static volatile int glob = 0;
static pthread_spinlock_t splock;

static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    int loops = *((int *) arg);
    int loc, s;

    for (int j = 0; j < loops; j++) {
        s = pthread_spin_lock(&splock);
        if (s != 0)
            errExitEN(s, "pthread_spin_lock");

        loc = glob;
        loc++;
        glob = loc;

        s = pthread_spin_unlock(&splock);
        if (s != 0)
            errExitEN(s, "pthread_spin_unlock");
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    int loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;

    int s = pthread_spin_init(&splock, 0);
    if (s != 0)
        errExitEN(s, "pthread_spin_init");

    pthread_t t1, t2;
    s = pthread_create(&t1, NULL, threadFunc, &loops);
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, &loops);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
