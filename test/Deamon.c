/*=========================================================
 *文件名称:  Deamon.c
 *创建日期：2017-1-24
 *修改记录：
 *  2017-1-24  首次创建
 *功能描述：写日志
 ===========================================================*/
#include "Deamon.h"

/*=========================================================
 *函数入参: 无 
 *函数出参: 无
 *返回值:   无
 *功能说明: 生成守护进程
 *===========================================================*/
/*
void SigtermHandler(int arg)
{
  _running = 0;
}
 */
void Daemon(void)
{
  pid_t                pid;
  int                  i;
  char                 errLog[LOGLEVELLEN];
  
  memset(errLog, 0, sizeof(errLog));
  pid = fork();
  if(pid < 0)
  {
    sprintf(errLog, "%s", "error fork\n");
    WriteSySLog(5, errLog);
    exit(1);
  }
  else if(pid > 0)
  {
    exit(0);
  }
  setsid();
  chdir("/btmsapp/btms/trans/bin");
  umask(0);
  for(i = 0; i < MAXFILE; i++)
  {
    close(i);
  }
  //signal(SIGTERM, SigtermHandler);
}
