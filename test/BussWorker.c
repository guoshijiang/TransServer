/*=========================================================
 *�ļ�����:  MultiBuss.h
 *�������ڣ�2017-1-23
 *�޸ļ�¼��
 *  2017-1-23  �״δ���
 *�������������ļ������º���
 *							ListenBankSecu     �������л���ȯ�̶�
 *							AcceptRecvBankSecu �������л���ȯ�̶˵���������
 *							RecvBank           �������ж˷������İ�
 *							SendBank           �������ж˷������İ�
 *							RecvNewSecu        �������������͹����İ�
 *							SendNewSecu        �������������͹����İ�
 *							RecvOldSecu        �������������͹����İ�
 *							SendOldSecu        �������������͹����İ�
 ===========================================================*/
#include "BussWorker.h"
 
int ListenBankSecu(char *iIp, char *iPort)
{
	int               recvBankSecuFd;
	char 							bankAddrIp[20];
	char              bankPort[10];
	int               socketPort;
	char              errLog[LOGLEVELLEN];
	memset(errLog, 0, sizeof(errLog));
	if(iIp == NULL && iPort == NULL)
	{
		sprintf(errLog, "%s", "iIP��iPortΪ��");
		WriteSySLog(2, errLog);
		return LISTENBANKSECUERR;
	}
	memset(bankAddrIp, 0, sizeof(bankAddrIp));
	sprintf(bankAddrIp, "%s", ReadConItem(iIp));
	memset(bankPort, 0, sizeof(bankPort));
	sprintf(bankPort, "%s", ReadConItem(iPort));
  socketPort = atoi(bankPort);
	recvBankSecuFd = InitListenSocket(bankAddrIp, socketPort);
	memset(errLog, 0, sizeof(errLog));
	if(recvBankSecuFd == 900 || recvBankSecuFd == 901 || recvBankSecuFd == 902 )
	{
		sprintf(errLog, "%s", "CInitListenSocket Fail\n");
		WriteSySLog(5, errLog);
		return LISTENBANKSECUERR;
	}
	return recvBankSecuFd;
}

int AcceptRecvBankSecu(int recvBankSecuFd)
{
	int  connfd;
	char errLog[LOGLEVELLEN];
	struct sockaddr_in cliaddr;
	socklen_t cliaddr_len;
	cliaddr_len = sizeof(cliaddr);
	connfd = Accept(recvBankSecuFd, (struct sockaddr *)&cliaddr, &cliaddr_len);
	memset(errLog, 0, sizeof(errLog));
	if(connfd < 0)
	{
		sprintf(errLog, "%s", "AcceptRecvBankSecu Fail\n");
		WriteSySLog(5, errLog);
		return LISTENBANKSECUERR;
	}
	return connfd;
}

int RecvBank(void *buf)
{
	int                  fifoFd;    //�ܵ����
	char                 logBuf[LOGLEVELLEN];  
	int                  lBankSceuFd;  //�������ж˷������ľ��
	ssize_t              wet;
	ssize_t              ret;          
	int                  aBankSecuFd;   //AcceptRecvBankSecu���ص�ֵ
	
	fifoFd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifoFd == -1)
	{
	 sprintf(logBuf, "%s", "�򿪹ܵ�ʧ��");
 	 WriteSySLog(3, logBuf);
   return FIFOIPC_OpenErr;
	}
	
	lBankSceuFd = ListenBankSecu("bankIP", "bankPort");
	memset(logBuf, 0, sizeof(logBuf));
	if(lBankSceuFd == LISTENBANKSECUERR)
	{
		sprintf(logBuf, "%s", "�������л���ȯ��ʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
	printf("�ȴ�����\n");
	aBankSecuFd = AcceptRecvBankSecu(lBankSceuFd);
	memset(logBuf, 0, sizeof(logBuf));
	if(aBankSecuFd  < 0)
	{
		sprintf(logBuf, "%s", "�������л���ȯ������ʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
  ret= read(aBankSecuFd, buf, MAXLINE);
  memset(logBuf, 0, sizeof(logBuf));
  if(ret == -1)
  {
  	sprintf(logBuf, "%s", "��ȡ���ж˷���������Ϣʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
  }
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "��ȡ���ж˷���������Ϣ�ɹ�", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(logBuf, 0, sizeof(logBuf));
	if(wet == -1)
	{
		sprintf(logBuf, "%s", "д�ܵ���������");
		WriteSySLog(4, logBuf);
		return WRITEFIFOERR;
	} 
	return ret; 
}

int SendBank(void *buf, int len)
{
	char 							bankAddrIp[20];
	char              bankPort[10];
	char              errLog[LOGLEVELLEN];
	char              logBuf[LOGLEVELLEN];
	int               sRetfd;
	int               fifoFd;
	int               wRet, wet;
	char              buftest[1024]; 
	
	memset(bankAddrIp, 0, sizeof(bankAddrIp));
	sprintf(bankAddrIp, "%s", ReadConItem("bankIP"));
	memset(bankPort, 0, sizeof(bankPort));
	sprintf(bankPort, "%s", ReadConItem("bankPort"));
	int port = atoi(bankPort);
	
	memset(errLog, 0, sizeof(errLog));
	sRetfd = Connect(bankAddrIp, port);
	if(sRetfd < 0)
	{
		sprintf(logBuf, "%s", "�������ж�ʧ��");
		WriteSySLog(4, errLog);
	  return CONNECTERR;
	} 
	
	memset(logBuf, 0, sizeof(logBuf));
	
  fgets(buftest, sizeof(buftest), stdin);
  wRet = write(sRetfd, buf, len);
  if(wRet < 0)
  {
  	sprintf(logBuf, "%s", "���͸����ж˵ı��ķ�������");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(errLog, 0, sizeof(errLog));
		if(wet == -1)
		{
			sprintf(errLog, "%s", "д�ܵ���������");
			WriteSySLog(4, errLog);
			return WRITEFIFOERR;
		}
  }             
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "���ͱ��ĸ����ж˳ɹ�", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(errLog, 0, sizeof(errLog));
	if(wet == -1)
	{
		sprintf(errLog, "%s", "д�ܵ���������");
		WriteSySLog(4, errLog);
		return WRITEFIFOERR;
	} 
	return wRet;
}

int RecvNewSecu(void *buf)
{
	int                  fifoFd;    //�ܵ����
	char                 logBuf[LOGLEVELLEN];  
	int                  lBankSceuFd;  //������ȯ�̶˷������ľ��
	ssize_t              wet;
	ssize_t              ret;          
	int                  aBankSecuFd;   //AcceptRecvBankSecu���ص�ֵ
	
	fifoFd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifoFd == -1)
	{
	 sprintf(logBuf, "%s", "�򿪹ܵ�ʧ��");
 	 WriteSySLog(3, logBuf);
   return FIFOIPC_OpenErr;
	}
	
	lBankSceuFd = ListenBankSecu("newSecuIp", "newSecuPort");
	memset(logBuf, 0, sizeof(logBuf));
	if(lBankSceuFd == LISTENBANKSECUERR)
	{
		sprintf(logBuf, "%s", "�������л���ȯ��ʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
	printf("�ȴ�����\n");
	aBankSecuFd = AcceptRecvBankSecu(lBankSceuFd);
	memset(logBuf, 0, sizeof(logBuf));
	if(aBankSecuFd  < 0)
	{
		sprintf(logBuf, "%s", "�������л���ȯ������ʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
  ret= read(aBankSecuFd, buf, MAXLINE);
  memset(logBuf, 0, sizeof(logBuf));
  if(ret == -1)
  {
  	sprintf(logBuf, "%s", "��ȡ��ȯ�̶˷���������Ϣʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
  }
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "��ȡ��ȯ�̶˷���������Ϣ�ɹ�", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(logBuf, 0, sizeof(logBuf));
	if(wet == -1)
	{
		sprintf(logBuf, "%s", "д�ܵ���������");
		WriteSySLog(4, logBuf);
		return WRITEFIFOERR;
	} 
	return ret; 
}

int SendNewSecu(void *buf, int len)
{
	char 							bankAddrIp[20];
	char              bankPort[10];
	char              errLog[LOGLEVELLEN];
	char              logBuf[LOGLEVELLEN];
	int               fifoFd;
	int               sRetfd;
	int               wRet, wet;
	
	memset(bankAddrIp, 0, sizeof(bankAddrIp));
	sprintf(bankAddrIp, "%s", ReadConItem("newSecuIp"));
	memset(bankPort, 0, sizeof(bankPort));
	sprintf(bankPort, "%s", ReadConItem("newSecuPort"));
	int port = atoi(bankPort);
	
	memset(errLog, 0, sizeof(errLog));
	sRetfd = Connect(bankAddrIp, port);
	if(sRetfd < 0)
	{
		sprintf(logBuf, "%s", "������ȯ�̶�ʧ��");
		WriteSySLog(4, errLog);
	  return CONNECTERR;
	} 
	
	memset(logBuf, 0, sizeof(logBuf));
  wRet = write(sRetfd, buf, len);
  if(wRet < 0)
  {
  	sprintf(logBuf, "%s", "���͸���ȯ�̶˵ı��ķ�������");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(errLog, 0, sizeof(errLog));
		if(wet == -1)
		{
			sprintf(errLog, "%s", "д�ܵ���������");
			WriteSySLog(4, errLog);
			return WRITEFIFOERR;
		}
  }             
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "���ͱ��ĸ���ȯ�̶˳ɹ�", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(errLog, 0, sizeof(errLog));
	if(wet == -1)
	{
		sprintf(errLog, "%s", "д�ܵ���������");
		WriteSySLog(4, errLog);
		return WRITEFIFOERR;
	} 
	return wRet;
}

int RecvOldSecu(void *buf)
{
	int                  fifoFd;    //�ܵ����
	char                 logBuf[LOGLEVELLEN];  
	int                  lBankSceuFd;  //������ȯ�̶˷������ľ��
	ssize_t              wet;
	ssize_t              ret;          
	int                  aBankSecuFd;   //AcceptRecvBankSecu���ص�ֵ
	
	fifoFd  = IPCFifoOpen("../ipc/myfifo", O_WRONLY);
	memset(logBuf, 0x00, sizeof(logBuf));
	if(fifoFd == -1)
	{
	 sprintf(logBuf, "%s", "�򿪹ܵ�ʧ��");
 	 WriteSySLog(3, logBuf);
   return FIFOIPC_OpenErr;
	}
	
	lBankSceuFd = ListenBankSecu("oldSecuIp", "oldSecuPort");
	memset(logBuf, 0, sizeof(logBuf));
	if(lBankSceuFd == LISTENBANKSECUERR)
	{
		sprintf(logBuf, "%s", "�������л���ȯ��ʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
	printf("�ȴ�����\n");
	aBankSecuFd = AcceptRecvBankSecu(lBankSceuFd);
	memset(logBuf, 0, sizeof(logBuf));
	if(aBankSecuFd  < 0)
	{
		sprintf(logBuf, "%s", "�������л���ȯ������ʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
	}
  ret= read(aBankSecuFd, buf, MAXLINE);
  memset(logBuf, 0, sizeof(logBuf));
  if(ret == -1)
  {
  	sprintf(logBuf, "%s", "��ȡ��ȯ�̶˷���������Ϣʧ��");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(logBuf, 0, sizeof(logBuf));
		if(wet == -1)
		{
			sprintf(logBuf, "%s", "д�ܵ���������");
			WriteSySLog(4, logBuf);
			return WRITEFIFOERR;
		}
  }
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "��ȡ��ȯ�̶˷���������Ϣ�ɹ�", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(logBuf, 0, sizeof(logBuf));
	if(wet == -1)
	{
		sprintf(logBuf, "%s", "д�ܵ���������");
		WriteSySLog(4, logBuf);
		return WRITEFIFOERR;
	} 
	return ret; 
}

int SendOldSecu(void *buf, int len)
{
	char 							bankAddrIp[20];
	char              bankPort[10];
	char              errLog[LOGLEVELLEN];
	char              logBuf[LOGLEVELLEN];
	int               sRetfd;
	int               fifoFd;
	int               wRet, wet;
	
	memset(bankAddrIp, 0, sizeof(bankAddrIp));
	sprintf(bankAddrIp, "%s", ReadConItem("oldSecuIp"));
	memset(bankPort, 0, sizeof(bankPort));
	sprintf(bankPort, "%s", ReadConItem("oldSecuPort"));
	int port = atoi(bankPort);
	
	memset(errLog, 0, sizeof(errLog));
	sRetfd = Connect(bankAddrIp, port);
	if(sRetfd < 0)
	{
		sprintf(logBuf, "%s", "�������ж�ʧ��");
		WriteSySLog(4, errLog);
	  return CONNECTERR;
	} 
	
	memset(logBuf, 0, sizeof(logBuf));
  wRet = write(sRetfd, buf, len);
  if(wRet < 0)
  {
  	sprintf(logBuf, "%s", "���͸���ȯ�̶˵ı��ķ�������");
		wet = write(fifoFd, logBuf, strlen(logBuf));
		memset(errLog, 0, sizeof(errLog));
		if(wet == -1)
		{
			sprintf(errLog, "%s", "д�ܵ���������");
			WriteSySLog(4, errLog);
			return WRITEFIFOERR;
		}
  }             
  memset(logBuf, 0, sizeof(logBuf));
	sprintf(logBuf, "%s %s", "���ͱ��ĸ���ȯ�̶˳ɹ�", buf); 
	wet = write(fifoFd, logBuf, strlen(logBuf));
	memset(errLog, 0, sizeof(errLog));
	if(wet == -1)
	{
		sprintf(errLog, "%s", "д�ܵ���������");
		WriteSySLog(4, errLog);
		return WRITEFIFOERR;
	} 
	return wRet;
}