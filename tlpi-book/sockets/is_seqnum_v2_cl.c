/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Solution for Exercise 59-2:c */

#include "is_seqnum_v2.h"

int
main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s server-host [sequence-len]\n", argv[0]);

    int cfd = inetConnect(argv[1], PORT_NUM_STR, SOCK_STREAM);
    if (cfd == -1)
        fatal("inetConnect() failed");

    char *reqLenStr = (argc > 2) ? argv[2] : "1";
    if (write(cfd, reqLenStr, strlen(reqLenStr)) !=  strlen(reqLenStr))
        fatal("Partial/failed write (reqLenStr)");
    if (write(cfd, "\n", 1) != 1)
        fatal("Partial/failed write (newline)");

    char seqNumStr[INT_LEN];            /* Start of granted sequence */
    ssize_t numRead = readLine(cfd, seqNumStr, INT_LEN);
    if (numRead == -1)
        errExit("readLine");
    if (numRead == 0)
        fatal("Unexpected EOF from server");

    printf("Sequence number: %s", seqNumStr);   /* Includes '\n' */

    exit(EXIT_SUCCESS);                         /* Closes 'cfd' */
}
