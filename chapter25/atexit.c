#include <stdlib.h>
#include <stdio.h>
#include "tlpi_hdr.h"

static void atexit_handler1(void) {
  printf("atexit_handler1\n");
}

static void atexit_handler2(void) {
  printf("atexit_handler2\n");
}

int main(int argc, char *argv[]) {
  printf("main\n");

  if (atexit(atexit_handler1) != 0) {
    errExit("atexit");
  }
  if (atexit(atexit_handler2) != 0) {
    errExit("atexit");
  }

  printf("atexit handlers registered\n");

  exit(EXIT_SUCCESS);

  printf("This should not be printed\n");
}