/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 61 */

/* scm_rights_send.c

   Used in conjunction with scm_rights_recv.c to demonstrate passing of
   file descriptors via a UNIX domain socket.

   This program sends a file descriptor to a UNIX domain socket.

   Usage is as shown in the usageErr() call below.

   File descriptors can be exchanged over stream or datagram sockets. This
   program uses stream sockets by default; the "-d" command-line option
   specifies that datagram sockets should be used instead.

   This program is Linux-specific.

   See also scm_multi_recv.c.
*/
#include "scm_rights.h"

int
main(int argc, char *argv[])
{

    /* Allocate a char array of suitable size to hold the ancillary data.
       However, since this buffer is in reality a 'struct cmsghdr', use a
       union to ensure that it is aligned as required for that structure.
       Alternatively, we could allocate the buffer using malloc(), which
       returns a buffer that satisfies the strictest alignment requirements
       of any type. */

    union {
        char   buf[CMSG_SPACE(sizeof(int))];
                        /* Space large enough to hold an 'int' */
        struct cmsghdr align;
    } controlMsg;

    /* Parse command-line options. */

    bool useDatagramSocket = false;
    bool verbose = false;
    int opt;

    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
        case 'd':
            useDatagramSocket = true;
            break;

        case 'v':
            verbose = true;
            break;

        default:
            usageErr("%s [options] file\n"
                    "    Options:\n"
                    "\t-d    Use datagram socket (default is stream)\n",
                    argv[0]);
        }
    }

    /* 'optind' is a global variable that is updated by getopt(). Upon
       completion of option processing, it contains the index of the next word
       in the command line following the options and option arguments. */

    if (argc != optind + 1)
        usageErr("%s [-d] file\n", argv[0]);

    /* Open the file named on the command line. */

    int fd = open(argv[optind], O_RDONLY);
    if (fd == -1)
        errExit("open");

    /* The 'msg_name' field can be used to specify the address of the
       destination socket when sending a datagram. However, we do not need to
       use this field because we use connect() below, which sets a default
       outgoing address for datagrams. */

    struct msghdr msgh;
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;

    /* We must transmit at least 1 byte of real data in order to send ancillary
       data. */

    struct iovec iov;
    int data = 12345;

    iov.iov_base = &data;
    iov.iov_len = sizeof(data);
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    if (verbose)
        printf("Sending data = %d\n", data);

    /* Set 'msgh' fields to describe the ancillary data buffer. */

    msgh.msg_control = controlMsg.buf;
    msgh.msg_controllen = sizeof(controlMsg.buf);

    /* The control message buffer must be zero-initialized in order for the
       CMSG_NXTHDR() macro to work correctly. Although we don't need to use
       CMSG_NXTHDR() in this example (because there is only one block of
       ancillary data), we show this step to demonstrate best practice. */

    memset(controlMsg.buf, 0, sizeof(controlMsg.buf));

    /* Set message header to describe the ancillary data that we want to
       send. */

    struct cmsghdr *cmsgp = CMSG_FIRSTHDR(&msgh);
    cmsgp->cmsg_len = CMSG_LEN(sizeof(int));
    cmsgp->cmsg_level = SOL_SOCKET;
    cmsgp->cmsg_type = SCM_RIGHTS;
    memcpy(CMSG_DATA(cmsgp), &fd, sizeof(int));

    /* Connect to the peer socket. */

    int sfd = unixConnect(SOCK_PATH,
                          useDatagramSocket ? SOCK_DGRAM : SOCK_STREAM);
    if (sfd == -1)
        errExit("unixConnect");

    printf("Sending FD %d\n", fd);

    /* Send real plus ancillary data. */

    ssize_t ns = sendmsg(sfd, &msgh, 0);
    if (ns == -1)
        errExit("sendmsg");

    if (verbose)
        printf("sendmsg() returned %zd\n", ns);

    /* Once the file descriptor has been sent, it is no longer necessary to
       keep it open in the sending process. */

    if (close(fd) == -1)
        errExit("close");

    exit(EXIT_SUCCESS);
}
