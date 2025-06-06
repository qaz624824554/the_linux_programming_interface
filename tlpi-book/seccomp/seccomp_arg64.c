/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2025.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter Z */

#define _GNU_SOURCE
#include <stddef.h>
#include <fcntl.h>
#include <linux/audit.h>
#include <sys/syscall.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <sys/prctl.h>
#include "tlpi_hdr.h"

/* The following is a hack to allow for systems (pre-Linux 4.14) that don't
   provide SECCOMP_RET_KILL_PROCESS, which kills (all threads in) a process.
   On those systems, define SECCOMP_RET_KILL_PROCESS as SECCOMP_RET_KILL
   (which simply kills the calling thread). */

#ifndef SECCOMP_RET_KILL_PROCESS
#define SECCOMP_RET_KILL_PROCESS SECCOMP_RET_KILL
#endif

static int
seccomp(unsigned int operation, unsigned int flags, void *args)
{
    return syscall(__NR_seccomp, operation, flags, args);
}

static void
install_filter(void)
{
    struct sock_filter filter[] = {

        /* Load architecture */

        BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                offsetof(struct seccomp_data, arch)),

        /* Kill the process if the architecture is not what we expect */

        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 0, 2),

        /* Load system call number */

        BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                 offsetof(struct seccomp_data, nr)),

        /* Kill the process if this is an x32 system call (bit 30 is set) */

#define X32_SYSCALL_BIT         0x40000000
        BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, X32_SYSCALL_BIT, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),

        /* Allow system calls other than lseek() */

        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_lseek, 1, 0),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),

        /* Load top 4 bytes of 'offset' argument; fail with errno==2 if
           these bytes are nonzero. The code here assumes a little-endian
           architecture (i.e., that the fist 4 bytes are the least
           significant bytes of the 64-bit argument and the following
           4 bytes are the most significant bytes). There are some macros
           in the kernel source file samples/seccomp/bpf-helper.h that show
           how endianess differences can be abstracted away when dealing
           with 64-bit arguments. */

        BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                 offsetof(struct seccomp_data, args[1]) + sizeof(__u32)),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0, 1, 0),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | 2),

        /* Load bottom 4 bytes of 'offset' argument; fail with errno==1
           if the value is > 1000; otherwise allow the system call */

        BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                 offsetof(struct seccomp_data, args[1])),
        BPF_JUMP(BPF_JMP | BPF_JGT | BPF_K, 1000, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | 1),

        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
    };

    struct sock_fprog prog = {
        .len = sizeof(filter) / sizeof(filter[0]),
        .filter = filter,
    };

    if (seccomp(SECCOMP_SET_MODE_FILTER, 0, &prog) == -1)
        errExit("seccomp");
}

static void
seek_test(int fd, off_t offset)
{
    printf("Seek to byte %lld: ", (long long) offset);
    off_t ret = lseek(fd, offset, SEEK_SET);
    if (ret == 0)
        printf("succeeded\n");
    else
        printf("failed with errno = %d\n", errno);
}

int
main(int argc, char *argv[])
{
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0))
        errExit("prctl");

    install_filter();

    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1)
        errExit("open");

    seek_test(fd, 0);
    seek_test(fd, 10000);
    seek_test(fd, 0x100000001);

    exit(EXIT_SUCCESS);
}
