#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {

  daemon(0, 0);

  sleep(30);

  exit(EXIT_SUCCESS);
}