/*=========================================================
 *文件名称:  WriteLog.c
 *创建日期：2017-1-24
 *修改记录：
 *  2017-1-24  首次创建
 *功能描述：写日志
 ===========================================================*/
#include "WriteLog.h"

char retime[10];
 
char *Timer()
{
	time_t ctime = time(0);
	struct tm tim = *localtime(&ctime);
	int day = tim.tm_mday;
	int month = tim.tm_mon + 1;
	int year = tim.tm_year + 1900;
	int second = tim.tm_sec;
	int minute = tim.tm_min;
	int hour = tim.tm_hour;
	sprintf(retime, "%d%02d%02d", year, month, day);
	return retime;
}

void WriteSySLog(const int level, char *log)
{
	char slog[LOGLEVELLEN];
	char fname[1024];
	memset(fname, 0, sizeof(fname));
	sprintf(fname, "%s/trans/log/%s_%s.log",getenv("HOME"),Timer(),"SYS");
	int fd = open(fname, O_WRONLY|O_APPEND|O_CREAT, 0666);
	if (fd != -1)
	{
		memset(slog, 0, sizeof(slog));
		switch(level)
		{
			case 1:
				sprintf(slog, "%s:-%s-%s", "SYS_LOG",Timer(), log);
				write(fd, slog, strlen(slog));
				break;
			case 2:
			case 3:
				sprintf(slog, "%s:-%s-%s", "INFO_LOG",Timer(), log);
				write(fd, slog, strlen(slog));
				break;
			case 4:
				sprintf(slog, "%s:-%s-%s", "DEBUG_LOG",Timer(), log);
				write(fd, slog, strlen(slog));
				break;
			case 5:
				sprintf(slog, "%s:-%s-%s", "ERROR_LOG",Timer(), log);
				write(fd, slog, strlen(slog));
				break;
			case 6:
				sprintf(slog, "%s:-%s-%s", "CORE_LOG",Timer(), log);
				write(fd, slog, strlen(slog));
				break;
			default:
				sprintf(slog, "%s:-%s-%s", "NO_LOG",Timer(), log);
				write(fd, slog, strlen(slog));
		}	
	}	
	close(fd);
}

void WriteTradeLog(char *log)
{
	char slog[LOGLEVELLEN];
	char fname[1024];
	memset(fname, 0, sizeof(fname));
	sprintf(fname, "%s/trans/log/%s_%s.log", getenv("HOME"), Timer(), "TRADE");
	int fd = open(fname, O_WRONLY|O_APPEND|O_CREAT, 0666);
	if (fd != -1)
	{
		memset(slog, 0, sizeof(slog));
		sprintf(slog, "%s-%s-%s", "TRADE_LOG",Timer(), log);
		write(fd, slog, strlen(slog));	
	}	
	close(fd);
}

int ReadLogFromFifo()
{
 int                  len, fd;
 char                 logContent[LOGLEVELLEN];
 char                 tradeLog[LOGLEVELLEN];
 char                 IPCfifo[100];
 
 memset(IPCfifo, 0, sizeof(IPCfifo));
 sprintf(IPCfifo, "%s/trans/ipc/%s", getenv("HOME"), "myfifo");
 
 memset(logContent, 0, sizeof(logContent));
 int ret = IPCMKFifo(IPCfifo, 0666);
 if(ret == -1)
 {
 		sprintf(logContent, "%s", "创建管道失败或者管道已经创建");
 		WriteSySLog(3, logContent);
 }
 memset(logContent, 0, sizeof(logContent));
 fd = IPCFifoOpen(IPCfifo, O_RDWR);
 if(fd == -1)
 {
 	 sprintf(logContent, "%s", "打开管道失败");
 	 WriteSySLog(3, logContent);
   return FIFOIPC_OpenErr;
 }
 memset(tradeLog, 0, sizeof(tradeLog));
 memset(logContent, 0, sizeof(logContent));
 while (1) 
 {
   len = read(fd, tradeLog, LOGLEVELLEN);
   if(len < 0)
   {
   	sprintf(logContent, "%s-%d", "从管道中读取数据失败",len);
   	WriteSySLog(1, logContent);
   	return LOGREADFAIL;
   }
   WriteTradeLog(tradeLog);
   memset(logContent, 0, sizeof(logContent));
   sprintf(logContent, "%s-%d", "从管道中读取数据成功",len);
   WriteSySLog(1, logContent);       
 }
 IPCFifoClose(fd);
 return 0;
}