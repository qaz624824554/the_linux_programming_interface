/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 15 */

#include <sys/stat.h>
#include <utime.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    char *pathname = argv[1];

    struct stat sb;
    if (stat(pathname, &sb) == -1)    /* Retrieve current file times */
        errExit("stat");

    struct utimbuf utb;
    utb.actime = sb.st_atime;         /* Leave access time unchanged */
    utb.modtime = sb.st_atime;        /* Make modify time same as access time */
    if (utime(pathname, &utb) == -1)  /* Update file times */
        errExit("utime");

    exit(EXIT_SUCCESS);
}
