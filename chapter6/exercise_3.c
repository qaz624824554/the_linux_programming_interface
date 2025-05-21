#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

/* 实现 setenv() 函数
 * 参数:
 *   name: 环境变量名
 *   value: 环境变量值
 *   overwrite: 如果为非零值，则覆盖已存在的变量；否则，如果变量已存在则不修改
 * 返回值:
 *   成功返回0，失败返回-1
 */
int my_setenv(const char *name, const char *value, int overwrite) {
  if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
    return -1; // 无效的环境变量名
  }

  // 检查环境变量是否已存在
  if (getenv(name) != NULL && overwrite == 0) {
    return 0; // 变量存在且不允许覆盖
  }

  // 构建 "name=value" 格式的字符串
  size_t len = strlen(name) + strlen(value) + 2; // +2 用于 '=' 和 '\0'
  char *string = malloc(len);
  if (string == NULL) {
    return -1; // 内存分配失败
  }

  snprintf(string, len, "%s=%s", name, value);

  // 使用 putenv() 设置环境变量
  int result = putenv(string);

  // 注意：不要释放 string，因为 putenv() 只存储指针
  // 如果释放 string，可能导致未定义行为

  return result;
}

/* 实现 unsetenv() 函数
 * 参数:
 *   name: 要删除的环境变量名
 * 返回值:
 *   成功返回0，失败返回-1
 */
int my_unsetenv(const char *name) {
  if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
    return -1; // 无效的环境变量名
  }

  size_t name_len = strlen(name);
  bool found = false;

  // 直接操作 environ 数组来删除所有匹配的环境变量
  char **env_ptr = environ;
  char **write_ptr = env_ptr;

  while (*env_ptr != NULL) {
    // 检查当前环境变量是否以 "name=" 开头
    if (strncmp(*env_ptr, name, name_len) == 0 && (*env_ptr)[name_len] == '=') {
      // 找到匹配项，跳过它（不复制到新位置）
      found = true;
      env_ptr++;
    } else {
      // 保留这个环境变量
      *write_ptr++ = *env_ptr++;
    }
  }

  // 确保数组以 NULL 结尾
  *write_ptr = NULL;

  return 0;
}

// 测试函数
void test_env_functions() {
  // 测试 setenv
  printf("Setting TEST_VAR=hello\n");
  my_setenv("TEST_VAR", "hello", 1);
  printf("TEST_VAR=%s\n", getenv("TEST_VAR"));

  // 测试不覆盖
  printf("Setting TEST_VAR=world with overwrite=0\n");
  my_setenv("TEST_VAR", "world", 0);
  printf("TEST_VAR=%s\n", getenv("TEST_VAR"));

  // 测试覆盖
  printf("Setting TEST_VAR=world with overwrite=1\n");
  my_setenv("TEST_VAR", "world", 1);
  printf("TEST_VAR=%s\n", getenv("TEST_VAR"));

  // 测试多次定义同一变量
  putenv("MULTI_VAR=value1");
  putenv("MULTI_VAR=value2");
  printf("Before unsetenv, MULTI_VAR=%s\n", getenv("MULTI_VAR"));

  // 测试 unsetenv
  printf("Unsetting MULTI_VAR\n");
  my_unsetenv("MULTI_VAR");
  printf("After unsetenv, MULTI_VAR=%s\n", getenv("MULTI_VAR"));

  // 测试 unsetenv 对 TEST_VAR
  printf("Unsetting TEST_VAR\n");
  my_unsetenv("TEST_VAR");
  printf("After unsetenv, TEST_VAR=%s\n", getenv("TEST_VAR"));
}

int main() {
  test_env_functions();
  return 0;
}