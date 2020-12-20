#include "def.h"
#include "init.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

int
main()
{
  setup();      // 安装信号处理函数
  shell_loop(); // 进入 shell 循环
  return 0;
}
