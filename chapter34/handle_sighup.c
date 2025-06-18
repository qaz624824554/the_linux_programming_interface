#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tlpi_hdr.h"

static void sig_hup(int signum) {
    printf("SIGUP received\n");
}

int main(int argc, char *argv[]) {
  signal(SIGHUP, sig_hup);

  sleep(100);
}