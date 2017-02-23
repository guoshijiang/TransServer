/*=========================================================
 *文件名称:  CmdParse.c
 *创建日期：2017-2-8
 *修改记录：
 *  2017-2-8  首次创建
 *功能描述：共享内存的公共函数封装
 ===========================================================*/
#include "CmdParse.h"

int CmdParse()
{
	char               cmdBuf[20];
	char               cmdLog[LOGLEVELLEN];
	int                wet, fifoFd, ret;
	
	fifoFd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(cmdLog, 0x00, sizeof(cmdLog));
	if(fifoFd == -1)
	{
	 sprintf(cmdLog, "%s", "打开管道失败");
 	 WriteSySLog(3, cmdLog);
   return FIFOIPC_OpenErr;
	}
	
	memset(cmdLog, 0, sizeof(cmdLog));
  if (!fgets(cmdBuf, 20, stdin))
  {
  	sprintf(cmdLog, "%s", "从屏幕上获取命令信息失败");
  	WriteSySLog(5, cmdLog);
  	exit(0);
  }
  memset(cmdLog, 0, sizeof(cmdLog));
	sprintf(cmdLog, "%s %s", "获取从屏幕获取命令信息成功:", cmdBuf);
	wet = write(fifoFd, cmdLog, strlen(cmdLog));
	memset(cmdLog, 0, sizeof(cmdLog));
	if(wet == -1)
	{
		sprintf(cmdLog, "%s", "写管道发生错误");
		WriteSySLog(4, cmdLog);
		return -1;
	}
	
	if (!fgets(cmdBuf, 20, stdin))
  {
  	exit(0);
  }
	
	return 0;
}


int main()
{
	pid_t pid;
	pid = fork();
	if(pid > 0)
	{
		ReadLogFromFifo();
	}
	else if(pid == 0)
	{
		sleep(1);
		CmdParse();
	}
	else
	{
		printf("出错了");	
	}
	return 0;	
}
