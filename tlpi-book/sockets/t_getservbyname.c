/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 59 */

#include <netdb.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s service [protocol]\n", argv[0]);

    struct servent *s = getservbyname(argv[1], argv[2]);
    if (s == NULL)
        fatal("getservbyname() could not find a matching record");

    printf("Official name: %s\n", s->s_name);
    printf("Aliases:      ");
    for (char **pp = s->s_aliases; *pp != NULL; pp++)
        printf(" %s", *pp);
    printf("\n");
    printf("Port:          %u\n", ntohs(s->s_port));
    printf("Protocol:      %s\n", s->s_proto);

    exit(EXIT_SUCCESS);
}
