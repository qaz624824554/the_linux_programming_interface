#include "tlpi_hdr.h"

// 无限循环执行
int main(int argc, char *argv[]) {
  while (1) {
    // printf("Hello, World!\n");
    sleep(1);
    sleep(1);
    sleep(1);
  }
  return 0;
}

// 实际上没有覆盖正在使用的文件，而是创建了一个新文件并给它相同的名字，而正在运行的程序继续使用旧文件的数据。