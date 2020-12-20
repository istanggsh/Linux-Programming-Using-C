#include "execute.h"
#include "def.h"
#include "init.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void
forkexec();

int
execute_disk_command()
{
  if (cmd_count == 0)
    return 0;

  int i;
  int fd, fds[2];

  // 第一条简单命令可能有输入重定向
  if (infile[0])
    cmd[0].infd = open(infile, O_RDONLY);

  // 最后一个命令可能有输出重定向
  if (outfile[0]) {
    if (append)
      cmd[cmd_count - 1].outfd =
        open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
    else
      cmd[cmd_count - 1].outfd =
        open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  }

  if (backgnd) // 后台进程忽略 SIGCHLD 避免 zombie进程
    signal(SIGCHLD, SIG_IGN);
  else // 前台进程恢复默认处理动作，父进程才能回收子进程
    signal(SIGCHLD, SIG_DFL);

  for (i = 0; i < cmd_count; i++) {
    if (i < cmd_count - 1) { // 如果不是最后一个简单命令，创建管道，并重定向
      pipe(fds);
      cmd[i].outfd = fds[1];
      cmd[i + 1].infd = fds[0];
    }

    forkexec(i);

    if ((fd = cmd[i].infd) != 0)
      close(fd);
    if ((fd = cmd[i].outfd) != 1)
      close(fd);
  }

  // 前台作业需要等待最后一个命令退出
  if (backgnd == 0)
    while (wait(NULL) != lastpid)
      ;
  return 0;
}

void
forkexec(int i)
{

  pid_t pid = fork();
  if (pid == -1)
    ERR_EXIT("fork");
  if (pid > 0) {
    if (backgnd)
      printf("%d\n", pid);
    lastpid = pid;
  } else {
    /* 将第一条简单命令（如果是后台命令的话）的 infd 重定向至 /dev/null
     * 当后台命令试图从标准输入获取数据时立即返回 EOF */
    if (cmd[i].infd == 0 && backgnd == 1)
      cmd[i].infd = open("/dev/null", O_RDONLY);

    // 将第一个简单命令子进程作为进程组组长
    if (i == 0)
      setpgid(0, 0);

    if (cmd[i].infd != 0)
      dup2(cmd[i].infd, 0);
    if (cmd[i].outfd != 1)
      dup2(cmd[i].outfd, 1);

    for (int j = 3; j < FOPEN_MAX; j++)
      close(j);

    // 恢复前台作业 SIGINT SIGQUIT 信号默认处理动作
    if (backgnd == 0) {
      signal(SIGINT, SIG_DFL);
      signal(SIGQUIT, SIG_DFL);
    }

    execvp(cmd[i].args[0], cmd[i].args);
    ERR_EXIT("execvp");
  }
}