/*=========================================================
 *�ļ�����:  CmdParse.c
 *�������ڣ�2017-2-8
 *�޸ļ�¼��
 *  2017-2-8  �״δ���
 *���������������ڴ�Ĺ���������װ
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
	 sprintf(cmdLog, "%s", "�򿪹ܵ�ʧ��");
 	 WriteSySLog(3, cmdLog);
   return FIFOIPC_OpenErr;
	}
	
	memset(cmdLog, 0, sizeof(cmdLog));
  if (!fgets(cmdBuf, 20, stdin))
  {
  	sprintf(cmdLog, "%s", "����Ļ�ϻ�ȡ������Ϣʧ��");
  	WriteSySLog(5, cmdLog);
  	exit(0);
  }
  memset(cmdLog, 0, sizeof(cmdLog));
	sprintf(cmdLog, "%s %s", "��ȡ����Ļ��ȡ������Ϣ�ɹ�:", cmdBuf);
	wet = write(fifoFd, cmdLog, strlen(cmdLog));
	memset(cmdLog, 0, sizeof(cmdLog));
	if(wet == -1)
	{
		sprintf(cmdLog, "%s", "д�ܵ���������");
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
		printf("������");	
	}
	return 0;	
}
