#include "builtin.h"
#include "def.h"
#include "init.h"
#include "parse.h"
#include <stdio.h>

typedef void (*CMD_HANDLER)();

typedef struct builtin_cmd
{
  char* name;
  CMD_HANDLER handler;
} BUILTIN_CMD;

void
do_exit();
void
do_cd();

BUILTIN_CMD builtins[] = { { "exit", do_exit },
                           { "cd", do_cd },
                           // {} add new command
                           { NULL, NULL } };

// 内部命令执行返回 1
int
builtin()
{
  int i = 0;
  while (builtins[i].name) {
    if (check(builtins[i].name)) {
      builtins[i].handler();
      return 1;
    }
    i++;
  }
  return 0;
}

void
do_exit()
{}

void
do_cd()
{}