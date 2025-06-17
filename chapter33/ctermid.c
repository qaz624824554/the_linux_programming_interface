#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    char *term = ctermid(NULL);
    if (term == NULL)
        errExit("ctermid");
    printf("当前终端: %s\n", term);
    exit(EXIT_SUCCESS);
}