#include "init.h"
#include "def.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>

void
sigint_handler(int sig)
{
  printf("\nminishell $ ");
  fflush(stdout);
}

void
setup()
{
  signal(SIGINT, sigint_handler);
  signal(SIGQUIT, SIG_IGN);
}

void
init()
{
  memset(cmd, 0, sizeof(cmd));
  for (int i = 0; i < PIPELINE; i++) {
    cmd[i].infd = 0;
    cmd[i].outfd = 1;
  }

  memset(cmdline, 0, sizeof(cmdline));
  memset(avline, 0, sizeof(avline));

  lineptr = cmdline;
  avptr = avline;

  memset(infile, 0, sizeof(infile));
  memset(outfile, 0, sizeof(outfile));

  cmd_count = 0;
  backgnd = 0;
  append = 0;
  lastpid = 0;

  printf("minishell $ "); // shell 程序提示符
  fflush(stdout);         // 刷新流，因为 printf 函数没有 \n
}