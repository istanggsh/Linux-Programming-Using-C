#include "parse.h"
#include "def.h"
#include "execute.h"
#include "init.h"
#include "builtin.h"
#include <stdio.h>

void
get_command(int i);
void
getname(char* name);

/**
 * shell 主循环
 */
void
shell_loop()
{
  while (1) {
    init();                   // 初始化环境
    if (read_command() == -1) // 读取命令
      break;
    parse_command();   // 解析命令
    execute_command(); // 执行命令
  }
  printf("\nexit\n");
}

/**
 * 读取命令
 * 成功返回 0，失败或者读取到 EOF 返回 -1
 */
int
read_command()
{
  // 按行读取命令，cmdline 包含 \n 字符
  if (fgets(cmdline, MAXLINE, stdin) == NULL)
    return -1;
  return 0;
}

/**
 * 解析命令
 * 成功返回解析到的命令个数，失败返回 -1
 */
int
parse_command()
{
  if (check("\n")) // 空命令直接返回
    return 0;

  if (builtin())
    return 0;

  /* cmd [ < filename ] [ | cmd ] ... [ [> | >>] filename ] [&] */
  // cat < test.txt | grep -n public > test2.txt &
  // 解析第一条简单命令
  get_command(0);
  // 判定是否有输入重定向符
  if (check("<")) {
    if (check("<"))
      append = 1;
    getname(infile);
  }
  // 判定是否有管道
  int i;
  for (i = 1; i < PIPELINE; ++i)
    if (check("|"))
      get_command(i);
    else
      break;
  // 判定是否有输出重定向符
  if (check(">"))
    getname(outfile);
  // 判定是否后台作业
  if (check("&"))
    backgnd = 1;
  // 判断命令结束 '\n'
  if (check("\n")) {
    cmd_count = i;
    return cmd_count;
  } else {
    fprintf(stderr, "Command line syntax error\n");
    return -1;
  }
  return 0;
}

/**
 * 执行命令
 * 成功返回 0，失败返回 -1
 */
int
execute_command()
{
  // 如果是内部命令

  execute_disk_command();
  return 0;
}

/**
 * 解析第 i 条简单命令至 cmd[i]
 * 提取 cmdline 中的命令参数至 avline 数组中，
 * 并且将 cmd 结构中的 args[] 中的每个指针指向解析出来的参数
 */
void
get_command(int i)
{
  int j = 0;
  int inword = 0;
  while (*lineptr != '\0') {
    while (*lineptr == ' ' || *lineptr == '\t')
      ++lineptr;

    cmd[i].args[j] = avptr;
    while (*lineptr != '\0' && *lineptr != ' ' && *lineptr != '>' &&
           *lineptr != '\t' && *lineptr != '<' && *lineptr != '|' &&
           *lineptr != '\n' && *lineptr != '&') {
      *avptr++ = *lineptr++;
      inword = 1;
    }
    *avptr++ = '\0';

    switch (*lineptr) {
      case ' ':
      case '\t':
        inword = 0;
        j++;
        break;
      case '<':
      case '>':
      case '|':
      case '&':
      case '\n':
        if (inword == 0)
          cmd[i].args[j] = NULL;
        return;
      default: /* for '\0' */
        return;
    }
  }
}

int
check(const char* str)
{
  char* p;
  while (*lineptr == ' ' || *lineptr == '\t')
    ++lineptr;

  p = lineptr;
  while (*str != '\0' && *str == *p) {
    ++str;
    ++p;
  }

  if (*str == '\0') {
    lineptr = p;
    return 1;
  }
  return 0;
}

void
getname(char* name)
{
  while (*lineptr == ' ' || *lineptr == '\t')
    ++lineptr;

  while (*lineptr != '\0' && *lineptr != ' ' && *lineptr != '>' &&
         *lineptr != '\t' && *lineptr != '<' && *lineptr != '|' &&
         *lineptr != '\n' && *lineptr != '&')
    *name++ = *lineptr++;
  *name++ = '\0';
}
