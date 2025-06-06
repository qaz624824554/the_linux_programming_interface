/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 39 */

#define _GNU_SOURCE
#include <sys/capability.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

int
main(int argc, char *argv[])
{
    /* Fetch and display process capabilities */

    cap_t caps = cap_get_proc();
    if (caps == NULL)
        errExit("cap_get_proc");

    char *str = cap_to_text(caps, NULL);
    if (str == NULL)
        errExit("cap_to_text");

    printf("Capabilities: %s\n", str);

    cap_free(caps);
    cap_free(str);

    /* If an argument was supplied, try to open that file */

    if (argc > 1) {
        int fd = open(argv[1], O_RDONLY);
        if (fd >= 0)
            printf("Successfully opened %s\n", argv[1]);
        else
            printf("Open failed: %s\n", strerror(errno));
    }

    exit(EXIT_SUCCESS);
}
