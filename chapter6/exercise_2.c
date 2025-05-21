
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static jmp_buf env;

void f1() {
  // segmentation fault (core dumped)
  if (setjmp(env) == 0) {
    printf("1\n");
  } else {
    printf("2\n");
  }
}

int main() {
  f1();
  longjmp(env, 2);
  printf("3\n");

  exit(EXIT_SUCCESS);
}