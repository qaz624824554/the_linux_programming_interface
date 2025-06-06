/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Solution for Exercise 45-2 */

#include <sys/ipc.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file-name keychar\n", argv[0]);

    printf("Size of key_t = %ld bytes\n", (long) sizeof(key_t));

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
        errExit("stat");

    key_t key = ftok(argv[1], argv[2][0]);
    if (key == -1)
        errExit("ftok");

    printf("Key = %lx i-node = %lx st_dev = %lx proj = %x\n",
          (unsigned long) key, (unsigned long) sb.st_ino,
          (unsigned long) sb.st_dev, (unsigned int) argv[2][0]);
    if (key == -1)
        printf("File does not exist\n");

    exit(EXIT_SUCCESS);
}
