#include "tlpi_hdr.h"

#define MAX_ALLOCS 1000000

int main(int argc, char *argv[]) {
  char *ptr[MAX_ALLOCS];
  int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

  printf("\n");

  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
  }

  numAllocs = getInt(argv[1], GN_GT_0, "num-allocs");

  if (numAllocs > MAX_ALLOCS) {
    cmdLineErr("num-allocs > %d\n", MAX_ALLOCS);
  }

  blockSize = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

  freeStep = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
  freeMin = (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
  freeMax = (argc > 5) ? getInt(argv[5], GN_GT_0, "max") : numAllocs;

  if (freeMax > numAllocs) {
    cmdLineErr("free-max > num-allocs\n");
  }

  printf("Initial program break: %10p\n", sbrk(0));

  printf("Allocating %d*%d bytes\n", numAllocs, blockSize);

  for (j = 0; j < numAllocs; j++) {
    // malloc()不会在每次被调用时都调用 sbrk()来调整 program break 的位置，而是周期性地分配大块内存，并从中将小片内存返回给调用者。
    ptr[j] = malloc(blockSize);
    printf("Malloc program break is now: %10p\n", sbrk(0));
    if (ptr[j] == NULL) {
      errExit("malloc");
    }
  }

  printf("Program break is now: %10p\n", sbrk(0));

  printf("Freeing blocks from %d to %d in steps of %d\n", freeMin, freeMax,
         freeStep);

  for (j = freeMin - 1; j < freeMax; j += freeStep) {
    free(ptr[j]);
  }

  printf("After free(), program break is: %10p\n", sbrk(0));

  exit(EXIT_SUCCESS);
}