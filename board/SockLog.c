/*=========================================================
 *文件名称:  SockLog.c
 *创建日期：2017-2-9
 *修改记录：
 *  2017-2-9  首次创建
 *功能描述：SockLog
 ===========================================================*/
#include "SockLog.h"

#define TRANS_DEBUG_FILE_	"sock.log"
#define TRANS_MAX_STRING_LEN 		10240

#define TRANS_NO_LOG_LEVEL			0
#define TRANS_DEBUG_LEVEL			  1
#define TRANS_INFO_LEVEL			  2
#define TRANS_WARNING_LEVEL		  3
#define TRANS_ERROR_LEVEL			  4

int  SocketLevel[5] = {TRANS_NO_LOG_LEVEL, TRANS_DEBUG_LEVEL, TRANS_INFO_LEVEL, TRANS_WARNING_LEVEL, TRANS_ERROR_LEVEL};
char TRANSLevelName[5][10] = {"NOLOG", "DEBUG", "INFO", "WARNING", "ERROR"};

static int TRANS_Error_GetCurTime(char* strTime)
{
	struct tm*	tmTime = NULL;
	size_t			timeLen = 0;
	time_t			tTime = 0;	
	tTime = time(NULL);
	tmTime = localtime(&tTime);
	timeLen = strftime(strTime, 33, "%Y.%m.%d %H:%M:%S", tmTime);
	return timeLen;
}

static int TRANS_Error_OpenFile(int* pf)
{
	char	fileName[1024];
	memset(fileName, 0, sizeof(fileName));
	sprintf(fileName, "%s/gsj/log/%s", getenv("HOME"), TRANS_DEBUG_FILE_);
  *pf = open(fileName, O_WRONLY|O_CREAT|O_APPEND, 0666);
  if(*pf < 0)
  {
      return -1;
  }
	return 0;
}

static void TRANS_Error_Core(const char *file, int line, int level, int status, const char *fmt, va_list args)
{
  char str[TRANS_MAX_STRING_LEN];
  int	 strLen = 0;
  char tmpStr[64];
  int	 tmpStrLen = 0;
  int  pf = 0;
  memset(str, 0, TRANS_MAX_STRING_LEN);
  memset(tmpStr, 0, 64);
  tmpStrLen = TRANS_Error_GetCurTime(tmpStr);
  tmpStrLen = sprintf(str, "[%s] ", tmpStr);
  strLen = tmpStrLen;
  tmpStrLen = sprintf(str+strLen, "[%s] ", TRANSLevelName[level]);
  strLen += tmpStrLen;
  if (status != 0) 
  {
      tmpStrLen = sprintf(str+strLen, "[ERRNO is %d] ", status);
  }
  else
  {
  	tmpStrLen = sprintf(str+strLen, "[SUCCESS] ");
  }
  strLen += tmpStrLen;
  tmpStrLen = vsprintf(str+strLen, fmt, args);
  strLen += tmpStrLen;
  tmpStrLen = sprintf(str+strLen, " [%s]", file);
  strLen += tmpStrLen;
  tmpStrLen = sprintf(str+strLen, " [%d]\n", line);
  strLen += tmpStrLen;
  if(TRANS_Error_OpenFile(&pf))
	{
		return ;
	}
  write(pf, str, strLen);
  close(pf);
  return ;
}

void Socket_Log(const char *file, int line, int level, int status, const char *fmt, ...)
{
  va_list args;
	if(level == TRANS_NO_LOG_LEVEL)
	{
		return;
	}
	va_start(args, fmt);
	TRANS_Error_Core(file, line, level, status, fmt, args);
	va_end(args);
	return ;
}
