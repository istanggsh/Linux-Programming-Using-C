#ifndef _DEF_H_
#define _DEF_H_

#include <stdlib.h>

#define MAXLINE 1024 // 输入行的最大长度
#define MAXARG 20    // 简单命令的参数上限
#define PIPELINE 5   // 管道行中简单命令上限
#define MAXNAME 100  // IO 重定向文件名的最大长度
#define ERR_EXIT(msg)                                                          \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

typedef struct command
{
  char* args[MAXARG + 1]; // 每条简单命令的参数列表，空指针结束
  int infd;
  int outfd;
} COMMAND;

char cmdline[MAXLINE + 1];
char avline[MAXLINE + 1];
char* lineptr;
char* avptr;

char infile[MAXNAME + 1];
char outfile[MAXNAME + 1];
COMMAND cmd[PIPELINE];

int cmd_count;
int backgnd;
int append;
int lastpid;

#endif /* _DEF_H_ */