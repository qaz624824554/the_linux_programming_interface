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

#include <sys/capability.h>
#include "tlpi_hdr.h"

#define PRCAP_SHOW_ALL          0x01    /* Also display capabilities that
                                           are not enabled in any set */
#define PRCAP_SHOW_UNRECOGNIZED 0x02    /* Display capabilities that are
                                           unrecognized by libcap */

static int
capIsSet(cap_t capSets, cap_value_t cap, cap_flag_t set)
{
    cap_flag_value_t value;

    if (cap_get_flag(capSets, cap, set, &value) == -1)
        errExit("cap_get_flag");

    return value == CAP_SET;
}

static int
capIsPermitted(cap_t capSets, cap_value_t cap)
{
    return capIsSet(capSets, cap, CAP_PERMITTED);
}

static int
capIsEffective(cap_t capSets, cap_value_t cap)
{
    return capIsSet(capSets, cap, CAP_EFFECTIVE);
}

static int
capIsInheritable(cap_t capSets, cap_value_t cap)
{
    return capIsSet(capSets, cap, CAP_INHERITABLE);
}

/* Print a line indicating whether the capability 'cap' is in the
   permitted, effective, and inheritable sets of the capabilities state
   provided in 'capSets'.
*/
static void
printCap(cap_t capSets, cap_value_t cap, char *capStrName, int flags)
{
    cap_flag_value_t dummy;

    if (cap_get_flag(capSets, cap, CAP_PERMITTED, &dummy) != -1) {
        if ((flags & PRCAP_SHOW_ALL) ||
                capIsPermitted(capSets, cap) ||
                capIsEffective(capSets, cap) ||
                capIsInheritable(capSets, cap))
            printf("%-22s %s%s%s\n", capStrName,
                   capIsPermitted(capSets, cap) ? "p" : " ",
                   capIsEffective(capSets, cap) ? "e" : " ",
                   capIsInheritable(capSets, cap) ? "i" : " ");
    } else {
        if (flags & PRCAP_SHOW_UNRECOGNIZED)
            printf("%-22s unrecognized by libcap\n", capStrName);
    }
}

static void
printAllCaps(cap_t capSets, int flags)
{
    printCap(capSets, CAP_AUDIT_CONTROL, "CAP_AUDIT_CONTROL", flags);
#ifdef CAP_AUDIT_READ           /* Since Linux 3.16 */
    printCap(capSets, CAP_AUDIT_READ, "CAP_AUDIT_READ", flags);
#endif
    printCap(capSets, CAP_AUDIT_WRITE, "CAP_AUDIT_WRITE", flags);
#ifdef CAP_BPF          /* Since Linux 5.8 */
    printCap(capSets, CAP_BPF, "CAP_BPF", flags);
#endif
#ifdef CAP_BLOCK_SUSPEND        /* Since Linux 3.5 */
    printCap(capSets, CAP_BLOCK_SUSPEND, "CAP_BLOCK_SUSPEND", flags);
#endif
#ifdef CAP_CHECKPOINT_RESTORE   /* Since Linux 5.9 */
    printCap(capSets, CAP_CHECKPOINT_RESTORE, "CAP_CHECKPOINT_RESTORE", flags);
#endif
    printCap(capSets, CAP_CHOWN, "CAP_CHOWN", flags);
    printCap(capSets, CAP_DAC_OVERRIDE, "CAP_DAC_OVERRIDE", flags);
    printCap(capSets, CAP_DAC_READ_SEARCH, "CAP_DAC_READ_SEARCH", flags);
    printCap(capSets, CAP_FOWNER, "CAP_FOWNER", flags);
    printCap(capSets, CAP_FSETID, "CAP_FSETID", flags);
    printCap(capSets, CAP_IPC_LOCK, "CAP_IPC_LOCK", flags);
    printCap(capSets, CAP_IPC_OWNER, "CAP_IPC_OWNER", flags);
    printCap(capSets, CAP_KILL, "CAP_KILL", flags);
    printCap(capSets, CAP_LEASE, "CAP_LEASE", flags);
    printCap(capSets, CAP_LINUX_IMMUTABLE, "CAP_LINUX_IMMUTABLE", flags);
#ifdef CAP_MAC_ADMIN            /* Since Linux 2.6.25 */
    printCap(capSets, CAP_MAC_ADMIN, "CAP_MAC_ADMIN", flags);
#endif
#ifdef CAP_MAC_OVERRIDE         /* Since Linux 2.6.25 */
    printCap(capSets, CAP_MAC_OVERRIDE, "CAP_MAC_OVERRIDE", flags);
#endif
    printCap(capSets, CAP_MKNOD, "CAP_MKNOD", flags);
    printCap(capSets, CAP_NET_ADMIN, "CAP_NET_ADMIN", flags);
    printCap(capSets, CAP_NET_BIND_SERVICE, "CAP_NET_BIND_SERVICE", flags);
    printCap(capSets, CAP_NET_BROADCAST, "CAP_NET_BROADCAST", flags);
    printCap(capSets, CAP_NET_RAW, "CAP_NET_RAW", flags);
#ifdef CAP_PERFMON              /* Since Linux 5.8 */
    printCap(capSets, CAP_PERFMON, "CAP_PERFMON", flags);
#endif
    printCap(capSets, CAP_SETGID, "CAP_SETGID", flags);
    printCap(capSets, CAP_SETFCAP, "CAP_SETFCAP", flags);
    printCap(capSets, CAP_SETPCAP, "CAP_SETPCAP", flags);
    printCap(capSets, CAP_SETUID, "CAP_SETUID", flags);
    printCap(capSets, CAP_SYS_ADMIN, "CAP_SYS_ADMIN", flags);
    printCap(capSets, CAP_SYS_BOOT, "CAP_SYS_BOOT", flags);
    printCap(capSets, CAP_SYS_CHROOT, "CAP_SYS_CHROOT", flags);
    printCap(capSets, CAP_SYS_MODULE, "CAP_SYS_MODULE", flags);
    printCap(capSets, CAP_SYS_NICE, "CAP_SYS_NICE", flags);
    printCap(capSets, CAP_SYS_PACCT, "CAP_SYS_PACCT", flags);
    printCap(capSets, CAP_SYS_PTRACE, "CAP_SYS_PTRACE", flags);
    printCap(capSets, CAP_SYS_RAWIO, "CAP_SYS_RAWIO", flags);
    printCap(capSets, CAP_SYS_RESOURCE, "CAP_SYS_RESOURCE", flags);
    printCap(capSets, CAP_SYS_TIME, "CAP_SYS_TIME", flags);
    printCap(capSets, CAP_SYS_TTY_CONFIG, "CAP_SYS_TTY_CONFIG", flags);
#ifdef CAP_SYSLOG               /* Since Linux 2.6.37 */
    printCap(capSets, CAP_SYSLOG, "CAP_SYSLOG", flags);
#endif
#ifdef CAP_WAKE_ALARM           /* Since Linux 3.0 */
    printCap(capSets, CAP_WAKE_ALARM, "CAP_WAKE_ALARM", flags);
#endif
}

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "%s <textual-cap-set>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    cap_t capSets = cap_from_text(argv[1]);
    if (capSets == NULL)
        errExit("cap_from_text");

    char *textCaps = cap_to_text(capSets, NULL);
    if (textCaps == NULL)
        errExit("cap_to_text");

    printf("caps_to_text() returned \"%s\"\n\n", textCaps);

    printAllCaps(capSets, PRCAP_SHOW_ALL);

    if (cap_free(textCaps) != 0 || cap_free(capSets) != 0)
        errExit("cap_free");

    exit(EXIT_SUCCESS);
}
